import os
import subprocess
import tkinter as tk
from tkinter import filedialog, messagebox
import matplotlib.pyplot as plt

# Function to handle file selection
def select_file():
    file_path = filedialog.askopenfilename()
    seed_input.delete(0, tk.END)
    seed_input.insert(0, file_path)

# Function to handle generating seed
def generate_seed():
    seed_input.delete(0, tk.END)
    # Get the current working directory
    current_dir = os.getcwd()
    # Construct the path to process_generator.out based on the current directory
    process_generator_path = os.path.join(current_dir, 'systemTests/test_generator.out')
    
    # Run process_generator.out with specified arguments
    command = [process_generator_path, processes_num.get()]
    seed_input.insert(0, f'{current_dir}/systemTests/test-{processes_num.get()}.txt')
    processes_div.grid_remove()

    try:
        subprocess.run(command, check=True)
    except subprocess.CalledProcessError as e:
        messagebox.showerror("Error", f"Failed to run process_generator.out: {e}")
        return

# Function to handle starting simulation
def start_simulation():
    command = ['make']
    
    try:
        subprocess.run(command, check=True)
    except subprocess.CalledProcessError as e:
        messagebox.showerror("Error", f"Failed to run process_generator.out: {e}")
        return
    
    seed = seed_input.get()
    sched_algo = scheduler_algo.get()
    time_slice_val = time_slice.get()
    
    if not seed:
        messagebox.showerror("Error", "Please select a seed file")
        return
    if not sched_algo:
        messagebox.showerror("Error", "Please select a scheduler algorithm")
        return
    if sched_algo == 'RR' and not time_slice_val:
        messagebox.showerror("Error", "Please enter a time slice")
        return
    
    if sched_algo == 'SRTN':
        sched_algo="0"
    elif sched_algo=='HPF':
        sched_algo="1"
    else:
        sched_algo="2"

    processes_div.grid_remove()
    start_btn.grid_remove()
    
    scheduler_status.grid()
    
    # Get the current working directory
    current_dir = os.getcwd()
    # Construct the path to process_generator.out based on the current directory
    process_generator_path = os.path.join(current_dir, 'gen.out')
    
    # Run process_generator.out with specified arguments
    command = [process_generator_path, seed, sched_algo]
    if sched_algo == 'RR':
        command.append(time_slice_val)
    
    try:
        subprocess.run(command, check=True)
        
        # Process the .perf and .log files
        perf_file_path = 'outputFiles/scheduler.perf'
        log_file_path = 'outputFiles/scheduler.log'
        
        with open(perf_file_path, 'r') as perf_file:
            perf_data = perf_file.readlines()

        # Extract data from the .perf file
        cpu_utilization = None
        avg_wta = None
        avg_waiting_time = None
        std_wta = None

        for line in perf_data:
            if line.startswith('CPU utilization'):
                cpu_utilization = float(line.split('=')[1].strip().split()[0])
            elif line.startswith('Avg WTA'):
                avg_wta = float(line.split('=')[1].strip())
            elif line.startswith('Avg Waiting'):
                avg_waiting_time = float(line.split('=')[1].strip())
            elif line.startswith('Std WTA'):
                std_wta = float(line.split('=')[1].strip())

        # Plot the data
        plt.figure(figsize=(10, 6))

        # Plot CPU Utilization
        plt.subplot(2, 2, 1)
        plt.bar(['CPU Utilization'], [cpu_utilization], color='blue')
        plt.title('CPU Utilization')
        plt.ylabel('Percentage')

        # Plot Average Weighted Turnaround Time
        plt.subplot(2, 2, 2)
        plt.bar(['Avg WTA'], [avg_wta], color='green')
        plt.title('Average Weighted Turnaround Time')
        plt.ylabel('Time')

        # Plot Average Waiting Time
        plt.subplot(2, 2, 3)
        plt.bar(['Avg Waiting Time'], [avg_waiting_time], color='orange')
        plt.title('Average Waiting Time')
        plt.ylabel('Time')

        # Plot Standard Deviation of Turnaround Time
        plt.subplot(2, 2, 4)
        plt.bar(['Std WTA'], [std_wta], color='red')
        plt.title('Standard Deviation of Turnaround Time')
        plt.ylabel('Time')

        plt.tight_layout()
        plt.savefig('outputFiles/perf_data_visualization.png')
        plt.show()
        # Read and process the .log file
        with open(log_file_path, 'r') as log_file:
            log_data = log_file.readlines()

        # Create a visual representation of the program lifecycle
        # Assuming log_data contains program lifecycle events
        process_lifecycle = []
        current_process = None

        for line in log_data:
            if 'started' in line:
                current_process = line.split()[3]
                process_lifecycle.append((current_process, 'started'))
            elif 'finished' in line:
                process_lifecycle.append((current_process, 'finished'))
                current_process = None
            elif 'stopped' in line:
                process_lifecycle.append((current_process, 'stopped'))
                current_process = None
            else:
                process_lifecycle.append((current_process, 'resumed'))

        # Visualize program lifecycle
        plt.figure(figsize=(10, 6))
        for idx, (process, event) in enumerate(process_lifecycle):
            if event == 'started':
                plt.scatter(idx, process, color='blue', label='started')
            elif event == 'finished':
                plt.scatter(idx, process, color='orange', label='finished')
            elif event == 'stopped':
                plt.scatter(idx, process, color='red', label='stopped')
            else :
                plt.scatter(idx, process, color='green', label='resumed')
        
        plt.yticks(range(1, len(process_lifecycle) + 1))
        plt.xlabel('Time')
        plt.ylabel('Process')
        plt.title('Program Lifecycle')
        plt.grid(True)
        plt.legend()
        plt.savefig('outputFiles/program_lifecycle.png')
        plt.show()
        

    except subprocess.CalledProcessError as e:
        messagebox.showerror("Error", f"Failed to run gen.out: {e}")
        return

# Create GUI
root = tk.Tk()
root.title("Ubuntastic")

container = tk.Frame(root)
container.pack()

# Gif container
gif_container = tk.Frame(container)
gif_container.grid(row=0, column=0)

# Input fields container
input_fields_container = tk.Frame(container)
input_fields_container.grid(row=0, column=1, padx=10)

seed_label = tk.Label(input_fields_container, text="Seed:")
seed_label.grid(row=0, column=0, sticky='w')

seed_input = tk.Entry(input_fields_container)
seed_input.grid(row=0, column=1, pady=5)

select_path_btn = tk.Button(input_fields_container, text="Select Existing Seed", command=select_file)
select_path_btn.grid(row=0, column=2)

make_path_btn = tk.Button(input_fields_container, text="Make New Seed", command=generate_seed)
make_path_btn.grid(row=0, column=3)

processes_div = tk.Frame(input_fields_container)
processes_div.grid(row=1, column=0, columnspan=4, pady=5)

processes_num_label = tk.Label(processes_div, text="Number of Processes:")
processes_num_label.grid(row=0, column=0, sticky='w')

processes_num = tk.Entry(processes_div)
processes_num.grid(row=0, column=1)

generate_seed_btn = tk.Button(processes_div, text="Generate Seed", command=generate_seed)
generate_seed_btn.grid(row=0, column=2)

scheduler_algo_label = tk.Label(input_fields_container, text="Algorithm:")
scheduler_algo_label.grid(row=2, column=0, sticky='w')

scheduler_algo = tk.StringVar()
scheduler_algo.set("SRTN")  # Default value

# Function to update scheduler_algo value when an option is selected
def update_scheduler_algo(*args):
    scheduler_algo.set(scheduler_algo_var.get())

scheduler_algo_var = tk.StringVar()
scheduler_algo_var.trace('w', update_scheduler_algo)

scheduler_algo_dropdown = tk.OptionMenu(input_fields_container, scheduler_algo_var, "SRTN", "HPF", "RR")
scheduler_algo_dropdown.grid(row=2, column=1, columnspan=3, pady=5, sticky='ew')

time_slice_label = tk.Label(input_fields_container, text="Time Slice:")
time_slice_label.grid(row=3, column=0, sticky='w')

time_slice = tk.Entry(input_fields_container)
time_slice.grid(row=3, column=1, pady=5)

start_btn = tk.Button(container, text="Start Simulation", command=start_simulation)
start_btn.grid(row=1, column=0, columnspan=2, pady=10)

scheduler_status = tk.Label(container, text="Waiting for our sleepy scheduler penguin to wake", bd=1, relief='raised', padx=5, pady=5)
scheduler_status.grid(row=2, column=0, columnspan=2, pady=10)
scheduler_status.grid_remove()

root.mainloop()
