const processesDiv = document.getElementById('processes-num-field');
const processesNum = document.getElementById('processes-num');
const schedulerStatus = document.getElementById('scheduler-status');
const seedInput = document.getElementById('seed');
const selectPath = document.getElementById('select-path');
const makeSeed = document.getElementById('make-path');
const algoSelect = document.getElementById('schedulerAlgo');
const timeSlice = document.getElementById('TimeSlice');
const timeSliceDiv = document.getElementById('time-slice-container');
const startBtn = document.getElementById('start-btn');
const InputContainer =document.getElementById('input-fields-container');
const generateSeed = document.getElementById('generate-seed');

selectPath.addEventListener('click', async function() {
    try {
        const handle = await window.showOpenFilePicker();
        const file = await handle[0].getFile();
        seedInput.value = file.path;
    } catch (error) {
        console.error('Error selecting file:', error);
    }
});

generateSeed.addEventListener('click',function() {
    console.log(request);
    seedInput.value = `test-${processesNum.value}.txt`;
    processesDiv.classList.add('hidden');
});

makeSeed.addEventListener('click', function() {
    seedInput.value = '';
    processesDiv.classList.remove('hidden');
});

algoSelect.addEventListener('change', function() {
    if (algoSelect.value === 'RR') {
        timeSliceDiv.classList.remove('hidden');
    } else {
        timeSliceDiv.classList.add('hidden');
        timeSlice.value = '';
    }
});

startBtn.addEventListener('click',function() {
    if(seedInput.value === '') {
        alert('Please select a seed file');
        return;
    }
    if(algoSelect.value === '') {
        alert('Please select a scheduler algorithm');
        return;
    }
    if(algoSelect.value === 'RR' && timeSlice.value === '') {
        alert('Please enter a time slice');
        return;
    }
    InputContainer.classList.add('hidden');
    startBtn.classList.add('hidden');        
    schedulerStatus.classList.remove('hidden');
});
