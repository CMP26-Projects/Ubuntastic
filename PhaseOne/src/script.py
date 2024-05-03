import os
import subprocess
import tkinter as tk
from tkinter import filedialog
import matplotlib.pyplot as plt

# Function to handle file selection
def select_file():
    file_path = filedialog.askopenfilename()
    seed_input.delete(0, tk.END)
    seed_input.insert(0, file_path)

# Function to handle generating seed
def generate_seed():
    seed_input.delete(0, tk.END)
    seed_input.insert(0, f'test-{processes_num.get()}.txt')
    processes_div.grid_remove()

# Function to handle starting simulation
def start_simulation():
    command = ['make']
    
    try:
        subprocess.run(command, check=True)
    except subprocess.CalledProcessError as e:
        tk.messagebox.showerror("Error", f"Failed to run process_generator.out: {e}")
        return
    
    command = ['clear']
    
    try:
        subprocess.run(command, check=True)
    except subprocess.CalledProcessError as e:
        tk.messagebox.showerror("Error", f"Failed to run process_generator.out: {e}")
        return
    
    seed = seed_input.get()
    sched_algo = scheduler_algo.get()
    time_slice_val = time_slice.get()
    
    if not seed:
        tk.messagebox.showerror("Error", "Please select a seed file")
        return
    if not sched_algo:
        tk.messagebox.showerror("Error", "Please select a scheduler algorithm")
        return
    if sched_algo == 'RR' and not time_slice_val:
        tk.messagebox.showerror("Error", "Please enter a time slice")
        return
    
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
        perf_file_path = 'output.perf'
        log_file_path = 'output.log'
        
        # Read and process the .perf file
        with open(perf_file_path, 'r') as perf_file:
            lines = perf_file.readlines()
            # Process the lines and create plots, images, etc.

        # Read and process the .log file
        with open(log_file_path, 'r') as log_file:
            lines = log_file.readlines()
            # Process the lines and create plots, images, etc.
        
        # Create plots, images, etc. (using matplotlib, PIL, etc.)
        # For example, let's create a simple plot
        plt.plot([1, 2, 3, 4])
        plt.ylabel('some numbers')
        plt.savefig('plot.png')  # Save the plot as an image
        
    except subprocess.CalledProcessError as e:
        tk.messagebox.showerror("Error", f"Failed to run gen.out: {e}")
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

scheduler_algo_dropdown = tk.OptionMenu(input_fields_container, scheduler_algo, "SRTN", "HPF", "RR")
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


