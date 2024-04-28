#include <gtk/gtk.h>

// Function to handle the start button click event
void on_start_button_clicked(GtkWidget *widget, gpointer data) {
    // Get the selected scheduling algorithm and file path from the input fields
    // Call your Scheduler simulation function with these inputs
    char *algorithm = g_strdup(gtk_entry_get_text(GTK_ENTRY(data->algorithm_entry)));
    char *quanta = g_strdup(gtk_entry_get_text(GTK_ENTRY(data->quanta_entry)));
    char *file_path = g_strdup(gtk_entry_get_text(GTK_ENTRY(data->file_entry)));
    
    printf("Converted string: %s\n", algorithm);
    printf("Converted string: %s\n", quanta);
    printf("Converted string: %s\n", file_path);
        
    // Call the Scheduler function with the provided algorithm and file path
    schedule_processes(algorithm, file_path,algorithm,quanta); // Example function call, replace with your Scheduler function
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *algorithm_label;
    GtkWidget *algorithm_entry;
    GtkWidget *quanta_label;
    GtkWidget *quanta_entry;
    GtkWidget *file_label;
    GtkWidget *file_entry;
    GtkWidget *start_button;

    gtk_init(&argc, &argv);

    // Create the main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Scheduler Simulation");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a grid layout
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Add algorithm selection label and entry
    algorithm_label = gtk_label_new("Scheduling Algorithm:");
    gtk_grid_attach(GTK_GRID(grid), algorithm_label, 0, 0, 1, 1);
    algorithm_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), algorithm_entry, 1, 0, 1, 1);


    // Add quanta label and entry
    quanta_label = gtk_label_new("Quanta:");
    gtk_grid_attach(GTK_GRID(grid), quanta_label, 0, 0, 1, 1);
    quanta_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), quanta_entry, 1, 0, 1, 1);

    // Add file path label and entry
    file_label = gtk_label_new("File Path:");
    gtk_grid_attach(GTK_GRID(grid), file_label, 0, 1, 1, 1);
    file_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), file_entry, 1, 1, 1, 1);

    // Add start button
    start_button = gtk_button_new_with_label("Start Simulation");
    g_signal_connect(start_button, "clicked", G_CALLBACK(on_start_button_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), start_button, 0, 2, 2, 1);

    // Show all widgets
    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}

