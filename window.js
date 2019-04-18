var DoubleSlider = require('double-slider')
const { ipcRenderer } = require('electron')
const { spawn } = require('child_process')
var three = require('three')
var OrbitControls = require('three-orbitcontrols')
var skip = require('./dev').skipToResults
var fs = require('fs'),
    readline = require('readline')
var Chart = require('chart.js')

var state = 'start'
var dir = './patients'
var patient_dirs, current_patient

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
    <button type="button" class="btn btn-primary btn-lg btn-block" id="start-astra">Start Astra</button>
    <button type="button" class="btn btn-primary btn-lg btn-block" id="load-data">Load Old Data</button>`

    for (patient of patient_dirs){
        $('#select-patient').append("<option>" + patient + "</option>")
    }
    $('#select-patient').append('<option>Create New Patient</option>')

    $('#select-patient').on('change', () => {
        displayPatientName()
        displayLoadData()
    })
    displayPatientName()
    displayLoadData()

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

        if (fs.existsSync(current_patient.dir + 'raw_data.txt')){
            fs.unlinkSync(current_patient.dir + 'raw_data.txt')
        }
        state = 'astra-running'
        update()
    })

    $('#load-data').on('click', () => {
        state = 'astra-done'
        current_patient = {
            "name": $('#select-patient')[0].value,
            "dir": dir + '/' + $('#select-patient')[0].value + '/'
        }
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

function displayLoadData(){
    if ($('#select-patient')[0].value === "Create New Patient"){
        $('#load-data').hide()
    }
    else {
        $('#load-data').show()
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
    camera.position.set(0, 0, 0)
    camera.lookAt(0, 0, 1)

    var renderer = new three.WebGLRenderer()
    renderer.setSize( display.width(), display.height())
    display.append(renderer.domElement);
    display.append('<button type="button" class="btn btn-primary btn-lg btn-block" id="astra-exit">Done</button>');
    resultsAnimate()

    body_tracker.stdout.on('data', (data) => {
        frame = JSON.parse(data)
        for (joint in frame.joints){
            if (frame.joints[joint].z <= 400){
                delete frame.joints[joint]
            }
        }
        fs.appendFileSync(current_patient.dir + "raw_data.txt", JSON.stringify(frame) + "\n")
        scene = addJoints(scene, frame)
        scene = addBones(scene, frame)
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
    // Declare slider variable so that it can be used within the scope of the whole results() function.
    var slider

    display.height(display.width() * 3 / 4)

    var scene = new three.Scene()
    var camera = new three.PerspectiveCamera(49.5, 4/3, 0.1, 8000)
    camera.position.set(0, 0, 0)
    camera.lookAt(0, 0, 1)

    var renderer = new three.WebGLRenderer()
    renderer.setSize( display.width(), display.height())

    controls = new three.OrbitControls( camera, renderer.domElement )

    controls.enableDamping = true
    controls.dampingFactor = 1

    controls.screenSpacePanning = false
    controls.maxPolarAngle = Math.PI / 2

    display.append(renderer.domElement);

    var geometry = new three.PlaneGeometry( 3000, 3000, 32 );
    var material = new three.MeshBasicMaterial( {color: 0x555555, side: three.DoubleSide} );
    var plane = new three.Mesh( geometry, material );
    plane.lookAt(0, 1, 0);

    (async () => {
        frames = await processResults()

        // Create the slider after processResults() so we can just re-use the frames.length property.
        display.append(
        `<div name = 'frame-slider' id = "frame-slider"
            data-min = "0"
            data-max = ${frames.length - 1}
            data-range = ${frames.length - 1}
        ></div>`);

        slider = new DoubleSlider(document.getElementById('frame-slider'));

        slider.addEventListener('slider:change', () => {
            console.log(`Min is: ${slider.value.min}, max is: ${slider.value.max}`);
        });
        resultsAnimate(frames, Number(slider.value.min))

        //graph of shoulder_angle vs. frames
        display.append(
          '<canvas id="shoulder_chart" width="400" height="400"></canvas>'
        );
        var ctx = document.getElementById('shoulder_chart').getContext('2d');
        console.log(slider)
        var chart_data = await getChartData(frames, slider.range);
        var chart_y = await getChartY(frames, slider.range);
        console.log(chart_y);
        var myChart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: chart_y,
                datasets: [{
                    data: chart_data,
                    label: 'Shoulder Angle',
                    backgroundColor: 'rgb(255, 99, 132)',
                    borderColor: 'rgb(255, 99, 132)',
                    fill: false
                }]
            },
            options: {
              title:{
                display: true,
                text: 'Shoulder Angle'
              },
              scales: {
                yAxes: [{
                  ticks: {
                    beginAtZero: true
                  }
                }]
              }
            }
        });

        controls.target = new three.Vector3(0, 0, frames.z_offset)
        plane.position.set(0, frames.y_offset, frames.z_offset)
        scene.add( plane );
    })()

    // This function needs to be defined inside the results() function
    // so that it can access the variables in this results()'s scope.
    function resultsAnimate(frames, frame_number){
        var frame = frames[frame_number]
        scene = addJoints(scene, frame)
        scene = addBones(scene, frame)
        controls.update()
        setTimeout( () => {
            requestAnimationFrame( () => {
				if (frame_number < Number(slider.value.max)){
                    resultsAnimate(frames, frame_number + 1)
                }
                else {
                    resultsAnimate(frames, Number(slider.value.min))
                }
            })
        }, frame.time)
        renderer.render(scene, camera)
    }
}

function getChartData(frames, max){

  return new Promise((resolve) => {
    var chart_data = []
    for (var i=0; i <= max; ++i){
      if (frames[i]["shoulder_angle"]){
          chart_data.push({"x": i, "y": frames[i]["shoulder_angle"]})
      }
    }
    resolve(chart_data)
  })
}

function getChartY(frames, max){
  return new Promise((resolve) => {
    var chart_y = []
    for (var i=0; i<=max; ++i){
      if (frames[i]["shoulder_angle"]){
        m = i.toString()
          chart_y.push(m)
      }
    }
    resolve(chart_y)
  })
}

function addJoints(scene, frame){
    var geometry = new three.SphereGeometry(30)
    var material = new three.MeshBasicMaterial( { color: 'white' } )
    var joints = frame.joints

    for (var joint in joints){
        scene.remove(scene.getObjectByName(joint))
        var point
        if (joint == "Head"){
            var mat = new three.MeshBasicMaterial( { color: 'red' } )
            var geo = new three.SphereGeometry(50)
            point = new three.Mesh(geo, mat)
        }
        else {
            point = new three.Mesh(geometry, material)
        }
        point.position.set(joints[joint].x, joints[joint].y, joints[joint].z)
        point.name = joint
        scene.add(point)
    }
    return scene
}

function addBones(scene, frame){
    while (scene.getObjectByName("bone")){
        scene.remove(scene.getObjectByName("bone"))
    }

    var createBone = (joint1, joint2) => {
        if (joint1 && joint2){
            var point1 = new three.Vector3(joint1.x, joint1.y, joint1.z)
            var point2 = new three.Vector3(joint2.x, joint2.y, joint2.z)
            var direction = new three.Vector3().subVectors(point2, point1)
            var helper = new three.ArrowHelper(direction.clone().normalize(), point1);

            var geometry = new three.CylinderGeometry(15, 15, direction.length(), 3, 1)
            var bone = new three.Mesh(geometry, new three.MeshBasicMaterial( { color: 'white' } ))

            bone.setRotationFromEuler(new three.Euler().setFromQuaternion(helper.quaternion))
            bone.position.set((point1.x + point2.x) / 2, (point1.y + point2.y) / 2, (point1.z + point2.z) / 2)
            bone.name = "bone"
            scene.add(bone)
        }
    }
    var joints = frame.joints
    createBone(joints.Head, joints.Neck)
    createBone(joints.Neck, joints["Spine Top"])
    createBone(joints["Spine Top"], joints["Spine Middle"])
    createBone(joints["Spine Middle"], joints["Spine Base"])
    createBone(joints["Spine Top"], joints["Left Shoulder"])
    createBone(joints["Spine Top"], joints["Right Shoulder"])
    createBone(joints["Left Shoulder"], joints["Left Elbow"])
    createBone(joints["Left Elbow"], joints["Left Wrist"])
    createBone(joints["Left Wrist"], joints["Left Hand"])
    createBone(joints["Right Shoulder"], joints["Right Elbow"])
    createBone(joints["Right Elbow"], joints["Right Wrist"])
    createBone(joints["Right Wrist"], joints["Right Hand"])
    createBone(joints["Spine Base"], joints["Left Hip"])
    createBone(joints["Spine Base"], joints["Right Hip"])
    createBone(joints["Left Hip"], joints["Left Knee"])
    createBone(joints["Left Knee"], joints["Left Foot"])
    createBone(joints["Right Hip"], joints["Right Knee"])
    createBone(joints["Right Knee"], joints["Right Foot"])

    return scene
}

function processResults(){
    return new Promise((resolve) => {
        var readStream = readline.createInterface({
            input: fs.createReadStream(current_patient.dir + 'raw_data.txt')
        })

        var frames = {}
        frames.length = 0
        frames.z_offset = 0
        frames.y_offset = 0
        var total_joints = 0

        readStream.on('line', (line) => {
            var frame = JSON.parse(line)
            frames[frames.length] = frame
            for (var joint in frame.joints){
                frames.z_offset += frame.joints[joint].z
                total_joints++
                if ((joint == "Right Foot" || joint == "Left Foot") && frame.joints[joint].y < frames.y_offset) {
                    frames.y_offset = frame.joints[joint].y
                }
            }
            frames.length++
        })

        readStream.on('close', () => {
            frames.z_offset = frames.z_offset / total_joints
            resolve(frames)
        })
    })
}
