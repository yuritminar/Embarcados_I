const { app, BrowserWindow } = require('electron');
const path = require('path');

let mainWindow;

app.on('ready', () => {
 mainWindow = new BrowserWindow({
  width: 800,
    height: 600,
    webPreferences: {
      nodeIntegration: true, // Habilita o uso do Node.js
      contextIsolation: false // Necessário para integração direta
    }
  });

  mainWindow.loadFile('index.html'); // Carrega a interface
});

// Fecha o app quando todas as janelas são fechadas
app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit();
  }
});