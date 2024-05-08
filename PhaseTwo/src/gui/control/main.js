const { app, BrowserWindow, ipcMain } = require('electron');
const path = require('node:path');
const { spawn } = require('child_process');

const createWindow = () => {
    const win = new BrowserWindow({
      width: 1380,
      height: 941,
      webPreferences: {
        preload: path.join(__dirname, 'preload.js')
      }
    });
    win.loadFile('./frontend/index.html')
};

app.whenReady().then(() => {
  createWindow();
  app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) createWindow();
  })
});

app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') app.quit()
});


function schedule(seed, schAlgo, timeSlice) {
  const process = spawn('../../system/process_generator.out', [seed, schAlgo, timeSlice]);

  process.stdout.on('data', (data) => {
      console.log(`stdout: ${data}`);
  });

  process.stderr.on('data', (data) => {
      console.error(`stderr: ${data}`);
  });

  process.on('close', (code) => {
      console.log(`child process exited with code ${code}`);
  });
}


function makeSeed(processNum) {
  const process = spawn('.../system/systemTests/test_generator.out', processNum);

  process.stdout.on('data', (data) => {
      console.log(`stdout: ${data}`);
  });

  process.stderr.on('data', (data) => {
      console.error(`stderr: ${data}`);
  });

  process.on('close', (code) => {
      console.log(`child process exited with code ${code}`);
  });
}

document.getElementById('start-btn').addEventListener('click', () => {
  const seed = document.getElementById('seed').value;
  const schAlgo = document.getElementById('schedulerAlgo').value;
  const timeSlice = document.getElementById('TimeSlice').value;
  schedule(seed, schAlgo, timeSlice);
});


document.getElementById('generate-seed').addEventListener('click', () => {
  let processesNum=document.getElementById('processes-num').value;
  makeSeed(processesNum);
});