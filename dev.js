module.exports = {
    skipToResults: function(){
        var patient = {
            "name": "patient-1",
            "dir": "./patients/patient-1/"
        }
    
        return {
            state: 'astra-done',
            current_patient: patient
        }
    }
}

