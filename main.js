const {app, BrowserWindow} = require('electron')

let win

app.on('ready', () =>{
    win = new BrowserWindow({width:800, height:600, show: false})

    win.loadFile('index.html')

    win.once('ready-to-show', () => {
        win.show()
    })

    win.on('closed', () => {
        win = null
    })
})