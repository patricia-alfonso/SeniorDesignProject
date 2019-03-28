const { ipcRenderer } = require('electron')
const { spawn } = require('child_process')
var three = require('three')
var skip = require('./dev').skipToResults
var fs = require('fs'),
    readline = require('readline')

var state = 'start'
var dir = './patients'
var patient_dirs, current_patient
// DEV ONLY
//var {state, current_patient} = skip()

getPatientDirs()

$(() => {
    console.log('document ready')
    update();
})

function getPatientDirs(){
    if (!fs.existsSync(dir)){
        fs.mkdirSync(dir)
    }
    else {
        patient_dirs = fs.readdirSync(dir)
    }
}

function update(){
    $('#main-display')[0].innerHTML = ''

    if (state == 'start'){
        start()
    }
    if (state == 'astra-running'){
        waiting()
    }
    if (state == 'astra-done'){
        results()
    }
    
    $('#quit').on('click', () => {
        ipcRenderer.send('quit')
    })

    $('#home').on('click', () => {
        state = 'start'
        update()
    })
}

function start(){
    getPatientDirs()

    $('#title-text')[0].innerHTML = "Welcome to Astra Body Tracker"
    
    $('#main-display')[0].innerHTML = `
    <div class="form-group">
        <label for="select-patient">Select Patient</label>
        <select class="form-control" id="select-patient"></select>
        <input class="form-control" type="text" placeholder="Patient Name" id="patient-name">
    </div>
    <button type="button" class="btn btn-primary btn-lg btn-block" id="start-astra">Start Astra</button>`

    for (patient of patient_dirs){
        $('#select-patient').append("<option>" + patient + "</option>")
    }
    $('#select-patient').append('<option>Create New Patient</option>')

    $('#select-patient').on('change', () => {
        displayPatientName()
    })
    displayPatientName()
    
    $('#start-astra').on('click', () => {
        console.log('start astra')

        var patient = $('#select-patient')[0].value
        if (patient === "Create New Patient"){
            patient = $('#patient-name')[0].value
            if (patient === ""){
                alert('Please provide a patient name')
                return
            }
        }

        current_patient = {
            "name": patient,
            "dir": dir + '/' + patient + '/'
        }

        state = 'astra-running'
        update()
    })
}

function displayPatientName(){
    if ($('#select-patient')[0].value === "Create New Patient"){
        $('#patient-name').show()
    }
    else {
        $('#patient-name').hide()
    }
}

function waiting(){
    $('#title-text')[0].innerHTML = "Astra is running..."

    if (!fs.existsSync(current_patient.dir)){
        fs.mkdirSync(current_patient.dir) 
    }

    const body_tracker = spawn(".\\astra-body-tracker\\x64\\Debug\\astra-body-tracker.exe", [current_patient.dir])

    var display = $('#main-display')
    display.height(display.width() * 3 / 4)

    var scene = new three.Scene()
    var camera = new three.PerspectiveCamera(49.5, 4/3, 0.1, 8000)

    var renderer = new three.WebGLRenderer()
    renderer.setSize( display.width(), display.height())
    display.append(renderer.domElement);
    display.append('<button type="button" class="btn btn-primary btn-lg btn-block" id="astra-exit">Done</button>');
    resultsAnimate()

    body_tracker.stdout.on('data', (data) => {
        //console.log('stdout:' + data)
        console.log(JSON.parse(data))

        fs.appendFileSync(current_patient.dir + "raw_data.txt", data)
        frame = JSON.parse(data)
        scene = addJoints(scene, frame)
    })

    $('#astra-exit').on('click', () => {
        body_tracker.kill()
    })

    body_tracker.on('close', (code) => {
        console.log('child process exited with code ' + code)
        state = 'astra-done'
        update()
    })

    function resultsAnimate(){
        requestAnimationFrame(resultsAnimate)
        renderer.render(scene, camera)
    }
}

function results(){
    $('#title-text')[0].innerHTML = "Astra Results: " + current_patient.name

    var display = $('#main-display')
    display.height(display.width() * 3 / 4)

    var scene = new three.Scene()
    var camera = new three.PerspectiveCamera(49.5, 4/3, 0.1, 8000)

    var renderer = new three.WebGLRenderer()
    renderer.setSize( display.width(), display.height())
    display.append(renderer.domElement);

    (async () => {
        frames = await processResults()
        resultsAnimate(frames, 0)
    })()

    // This function needs to be defined inside the results() function 
    // so that it can access the variables in this results()'s scope.
    function resultsAnimate(frames, frame_number){
        var frame = frames[frame_number]
        scene = addJoints(scene, frame)
        setTimeout( () => {
            requestAnimationFrame( () => {
                if (frame_number < frames.length - 1){
                    resultsAnimate(frames, frame_number + 1)
                }
                else {
                    resultsAnimate(frames, 0)
                }
            })
        }, frame.time)
        renderer.render(scene, camera)
    }
}

function addJoints(scene, frame){
    var geometry = new three.SphereGeometry(30)
    var material = new three.MeshBasicMaterial( { color: 'white' } )
    var joints = frame.joints
    
    while (scene.children.length > 0){
        scene.remove(scene.children[0])
    }

    for (var joint in joints){
        var point
        if (joint == "Head"){
            var mat = new three.MeshBasicMaterial( { color: 'red' } )
            var geo = new three.SphereGeometry(50)
            point = new three.Mesh(geo, mat)
        }
        else {
            point = new three.Mesh(geometry, material)
        }
        point.position.set(joints[joint].x, joints[joint].y, -joints[joint].z)
        point.name = joint
        scene.add(point)
    }
    return scene
}

function processResults(){
    return new Promise((resolve) => {
        var readStream = readline.createInterface({
            input: fs.createReadStream(current_patient.dir + 'raw_data.txt')
        })
        
        var frames = []
        
        readStream.on('line', (line) => {
            var frame = JSON.parse(line)
            frames.push(frame)
        })

        readStream.on('close', () => {
            resolve(frames)
        })
    })
}