const {spawn} = require('child_process')

$(() => {
    

    $('#start-astra').on('click', function() {
        var patient = $('#patient-name')[0].value
        var output = $('#output-dir')[0].value
        
        if (output[output.length - 1] != '\\'){
            output = output + '\\'
        }

        const p = spawn(".\\astra-body-tracker\\x64\\Debug\\astra-body-tracker.exe", [patient, output])

        p.stdout.on('data', (data) => {
            console.log('stdout:' + data)
        })

        p.on('close', (code) => {
            console.log('child process exited with code ' + code)
        })
    })
})