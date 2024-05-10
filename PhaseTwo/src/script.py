
import tkinter as tk
from tkinter import filedialog, messagebox, ttk
import subprocess
import os
import matplotlib.pyplot as plt
from PIL import Image, ImageDraw, ImageFont, ImageGrab
def select_file():
    file_path = filedialog.askopenfilename()
    seed_input.delete(0, tk.END)
    seed_input.insert(0, file_path)

def start_simulation():
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

    start_btn.grid_remove()
    scheduler_status.grid()
    
    command = ['make']
    try:
        subprocess.run(command, check=True)
    except subprocess.CalledProcessError as e:
        messagebox.showerror("Error", f"Failed to run process_generator.out: {e}")
        return
    
    current_dir = os.getcwd()
    process_generator_path = os.path.join(current_dir, 'gen.out')
    command = [process_generator_path, seed, sched_algo, time_slice_val]
    try:
        subprocess.run(command, check=True)
        perf_file_path = os.path.join(current_dir, 'outputFiles/scheduler.perf')
        log_file_path = os.path.join(current_dir, 'outputFiles/scheduler.log')
        mem_file_path = os.path.join(current_dir, 'outputFiles/memory.log')
        text_to_image(perf_file_path, 'outputFiles/perf_image.png')
        text_to_image(mem_file_path, 'outputFiles/mem_image.png')
        text_to_image(log_file_path, 'outputFiles/log_image.png')        
        create_perf_img(perf_file_path)
        create_log_img(log_file_path)        
        create_mem_img(mem_file_path)
        return        
    except subprocess.CalledProcessError as e:
        messagebox.showerror("Error", f"Failed to run gen.out: {e}")
        return

def create_log_img(log_file_path):
    process_lifecycle = []
    with open(log_file_path, 'r') as log_file:
        log_data = log_file.readlines()
    for line in log_data:
        parts = line.split()
        time = parts[2]
        process_id = parts[4]
        state = parts[6]
        process_lifecycle.append((time, process_id, state))

    table_window = tk.Toplevel()
    table_window.title("Process Timeline")
    
    tree = ttk.Treeview(table_window)
    tree['show'] = 'headings'
    
    tree["columns"] = ("Time", "Process ID", "Process State")
    tree.column("Time", anchor='center', width=100)
    tree.column("Process ID", anchor='center', width=100)
    tree.column("Process State", anchor='center', width=150)
    
    tree.heading("Time", text="Time")
    tree.heading("Process ID", text="Process ID")
    tree.heading("Process State", text="Process State")
    
    for idx, (time, process_id, state) in enumerate(process_lifecycle):
        color = 'white'  # Default color
        
        if state == 'started' or state == 'resumed':
            color = 'green'
        elif state == 'stopped':
            color = 'red'
        elif state == 'finished':
            color='blue'
            
        tree.insert("", idx, values=(time, process_id, state), tags=(f'{idx}',))
        
        tree.tag_configure(f'{idx}', background=color)
    
    scrollbar = ttk.Scrollbar(table_window, orient="vertical", command=tree.yview)
    tree.configure(yscrollcommand=scrollbar.set)
    scrollbar.pack(side="right", fill="y")
    
    tree.pack(expand=True, fill="both")
    table_window.update()  # Ensure all widgets are updated
    return


def text_to_image(text_file, output_image):
    with open(text_file, 'r') as f:
        lines = f.readlines()

    font_size = 20
    line_height = 20
    font = ImageFont.load_default()
    image_width = 800
    image_height = len(lines) * line_height

    # Create a new image
    image = Image.new('RGB', (image_width, image_height), color = (255, 255, 255))
    draw = ImageDraw.Draw(image)

    # Write text to the image
    y_position = 0
    for line in lines:
        draw.text((0, y_position), line, fill=(0, 0, 0), font=font)
        y_position += line_height

    # Save the image
    image.save(output_image)

def create_mem_img(mem_file_path):
    mem_lifecycle = []
    with open(mem_file_path, 'r') as mem_file:
        mem_data = mem_file.readlines()
    for line in mem_data:
        parts = line.split()
        time = parts[2]
        action = parts[3]
        bytes = parts[4]
        process_id = parts[8]
        start_index = parts[10]
        end_index = parts[12]
        mem_lifecycle.append((time, process_id, action,
                               bytes, start_index, end_index))
        
    table_window = tk.Toplevel()
    table_window.title("Memory Timeline")
    
    tree = ttk.Treeview(table_window)
    tree['show'] = 'headings'
    
    tree["columns"] = ("Time", "Process ID", "Action", "Bytes","From","To")
    tree.column("Time", anchor='center', width=100)
    tree.column("Process ID", anchor='center', width=100)
    tree.column("Action", anchor='center', width=150)
    tree.column("Bytes", anchor='center', width=100)
    tree.column("From", anchor='center', width=100)
    tree.column("To", anchor='center', width=100)


    tree.heading("Time", text="Time")
    tree.heading("Process ID", text="Process ID")
    tree.heading("Action", text="Action")
    tree.heading("Bytes", text="Bytes")
    tree.heading("From", text="From")
    tree.heading("To", text="To")
    
    for idx, (time, process_id, action,bytes, 
              start_index, end_index)in enumerate(mem_lifecycle):
        color = 'white'  # Default color
        
        if action == 'allocated':
            color = 'orange'
        else:
            color='blue'
            
        tree.insert("", idx, values=(time, process_id, action,
                               bytes, start_index, end_index), tags=(f'{idx}',))
        
        tree.tag_configure(f'{idx}', background=color)
    
    scrollbar = ttk.Scrollbar(table_window, orient="vertical", command=tree.yview)
    tree.configure(yscrollcommand=scrollbar.set)
    scrollbar.pack(side="right", fill="y")
    
    tree.pack(expand=True, fill="both")
    table_window.update()  # Ensure all widgets are updated
    return

def create_perf_img(perf_file_path):
    with open(perf_file_path, 'r') as perf_file:
        perf_data = perf_file.readlines()

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

    plt.figure(figsize=(10, 6))

    plt.subplot(2, 2, 1)
    plt.bar(['CPU Utilization'], [cpu_utilization], color='blue')
    plt.title('CPU Utilization')
    plt.ylabel('Percentage')

    plt.subplot(2, 2, 2)
    plt.bar(['Avg WTA'], [avg_wta], color='green')
    plt.title('Average Weighted Turnaround Time')
    plt.ylabel('Time')

    plt.subplot(2, 2, 3)
    plt.bar(['Avg Waiting Time'], [avg_waiting_time], color='orange')
    plt.title('Average Waiting Time')
    plt.ylabel('Time')

    plt.subplot(2, 2, 4)
    plt.bar(['Std WTA'], [std_wta], color='red')
    plt.title('Standard Deviation of Turnaround Time')
    plt.ylabel('Time')

    plt.tight_layout()
    plt.savefig('outputFiles/perf_data_visualization.png')
    plt.show()
    return

# Create GUI
root = tk.Tk()
root.title("Ubuntastic")

container = tk.Frame(root)
container.pack()
#At time x allocated y bytes for process z from i to j

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

scheduler_algo_label = tk.Label(input_fields_container, text="Algorithm:")
scheduler_algo_label.grid(row=2, column=0, sticky='w')

scheduler_algo = tk.StringVar()
scheduler_algo.set("SRTN")  # Default value

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
