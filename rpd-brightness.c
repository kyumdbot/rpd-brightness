#define _GNU_SOURCE
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

GtkWidget *spin_btn;


int read_value() {
	FILE *fp;
	char ch;
	fp = fopen ("/sys/class/backlight/nv_backlight/brightness", "rb");
	if (fp) {
		char numstr[4];
		int idx = 0;
		while (1) {
			ch = fgetc (fp);
			if (ch == EOF || idx >= 3) break;
			else if (ch == '\n' || ch == ' ' || ch == '\0' || ch == '\t') continue;
			else numstr[idx] = ch; idx++; 
		}
		fclose (fp);
		numstr[idx] = '\0';
		int num = atoi(numstr);
		//printf("value: %d\n", num);
		return num;
	}
	return 0;
}

void end_program(GtkWidget *wid, gpointer ptr) {
	gtk_main_quit();
}

void value_changed(GtkWidget *wid, gpointer ptr) {
	const char *input = gtk_entry_get_text(GTK_ENTRY(spin_btn));
	char* cmd;
	int value = atoi(input);
	asprintf( &cmd, "echo %d > /sys/class/backlight/nv_backlight/brightness", value );
	//printf("run: %s\n", cmd);
	system(cmd);
}

void main(int argc, char *argv[]) {
	int current_value = read_value();
	gtk_init(&argc, &argv);
	
	GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *lbl = gtk_label_new("  Display Brightness :  ");
	GtkObject *adj = gtk_adjustment_new(0, -10, 10, 1, 0, 0);
	spin_btn = gtk_spin_button_new(GTK_ADJUSTMENT(adj), 0, 0);
	gtk_spin_button_set_range((GtkSpinButton *)spin_btn, 1, 99);
	gtk_spin_button_set_value((GtkSpinButton *)spin_btn, current_value);
	
	g_signal_connect(win, "delete_event", G_CALLBACK(end_program), NULL);
	g_signal_connect(spin_btn, "value-changed", G_CALLBACK(value_changed), lbl);
	
	GtkWidget *tab = gtk_table_new(3, 4, TRUE);
	gtk_table_attach_defaults(GTK_TABLE(tab), lbl, 0, 2, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(tab), spin_btn, 2, 3, 1, 2);
	gtk_container_add(GTK_CONTAINER(win), tab);
	
	gtk_widget_show_all(win);
	gtk_main();
}
