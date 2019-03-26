const electron = require('electron')
const { app, BrowserWindow } = electron
const { ipcMain } = require('electron')

let win

app.on('ready', () =>{
    const { width, height } = electron.screen.getPrimaryDisplay().workAreaSize
    win = new BrowserWindow({
        width: width / 2, 
        height: height * 3 / 4, 
        show: false,
        frame: false
    })

    win.loadFile('index.html')

    // DEV ONLY
    win.toggleDevTools();

    win.once('ready-to-show', () => {
        win.show()
    })

    win.on('closed', () => {
        win = null
    })
})

ipcMain.on('quit', () => {
    app.quit()
})