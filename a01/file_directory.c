/*
 -------------------------------------
 File:    file_directory.c
 Project: a01
 Operating Systems: Q1
 -------------------------------------
 Author:  Mila Cvetanovska
 ID:      210311400
 Email:   cvet1400@mylaurier.ca
 Version  2023-09-21
 -------------------------------------
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>

// 1.
// Create a directory with a given name and permissions.
void create_directory(const char *dir_name, mode_t mode) {

	int dir = mkdir(dir_name, mode);
	if (dir == -1) {
		printf("Error: failed to create a directory\n");
	} else {
		printf("The directory test is created successfully with mode %o\n",
				umask(mode));
	}
}

// 2.
// Create a file with a given name and write "Operating System is Fun!!" to the file.
void create_write_file(const char *file_name, const char *content) {
	size_t size = strlen(content);
	int fd = creat(file_name, O_WRONLY);
	if (fd == -1) {
		printf("Error: failed to create file");
	} else {
		write(fd, content, size);
		printf("File %s is created and written successfully", file_name);
	}
	close(fd);
}

// 3.
// Get the working directory path.
void get_working_dir(void) {

	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) == NULL) {
		printf("Error: failed to get working directory");
	} else {
		printf("The current working directory is: %s", cwd);
	}
}

// 4.
// Extract a terse list of what package actions occurred and save last 10 lines of that file to a (.log) file.
// Hint: you should use popen() to run the system command in a C program.
void read_historylog_to_file(const char *log_file_name) {

	char command[256];
	char buffer[256];

	// Define the command to extract the terse list of package actions
	snprintf(command, sizeof(command), "tail -n 10 /var/log/apt/history.log");

	// Open the history log file for reading
	FILE *history_log = popen(command, "r");
	if (history_log == NULL) {
		printf("Error: cannot open history log file");
	}

	// Open the output (.log) file for writing
	FILE *outputFile = fopen(log_file_name, "w");
	if (outputFile == NULL) {
		printf("Error: cannot open output log file");
	}

	// Read lines from the history log and write them to the output log
	while (fgets(buffer, sizeof(buffer), history_log) != NULL) {
		fprintf(outputFile, "%s", buffer);
	}

	printf(
			"The last 10 lines of /var/log/apt/history.log are copied to %s successfully",
			log_file_name);

	// Close the pipe and the output file
	pclose(history_log);
	fclose(outputFile);
}

// 5.
// Extract and save key information about the system's memory from /proc/meminfo to a file.
void read_proc_mem_info_to_file(const char *file) {

	// Open /proc/meminfo for reading
	FILE *meminfo_file = fopen("/proc/meminfo", "r");
	if (meminfo_file == NULL) {
		printf("Error: cannot open /proc/meminfo");
	}

	// Open or create the output file for writing
	FILE *output_file = fopen(file, "w");
	if (output_file == NULL) {
		printf("Error: cannot open or create the output file");
		fclose(meminfo_file);
	}

	// Read each line from /proc/meminfo and write it to the output file
	char line[256];
	while (fgets(line, sizeof(line), meminfo_file) != NULL) {
		// Write the line to the output file
		fprintf(output_file, "%s", line);
	}

	// Close the files
	fclose(meminfo_file);
	fclose(output_file);

	printf("Proc info is written to %s\n", file);
}

// 6.
// Create a subdirectory of the given name and permissions and move a file (must exist) to this subdirectory.
// Hint: you should use rename() function for moving the file.
void create_subdirectory_and_move_file(const char *sub_dir_name, mode_t mode,
		const char *file_name) {

	// Create the subdirectory with the specified permissions
	int dir = mkdir(sub_dir_name, mode);
	if (dir == -1) {
		printf("Error: failed to create a directory");
	} else {
		printf("Directory %s created successfully with mode %d:\n",
				sub_dir_name, mode);
		// Construct the full path to the new location of the file
		char new_file_path[256];
		snprintf(new_file_path, sizeof(new_file_path), "%s/%s", sub_dir_name,
				file_name);

		// Move the file to the subdirectory using rename()
		if (rename(file_name, new_file_path) != 0) {
			printf("Error: cannot move the file");
		} else {
			printf(
					"The file %s has been moved successfully to the sub-directory.\n",
					file_name);
		}

	}
}

// 7.
// Change the working directory to the given name.
void change_directory(const char *dir_name) {
	if (chdir(dir_name) == -1) {
		printf("Error: failed to change directory.\n");
	} else {
//		char buf[BUFSIZ];
//		strcpy(dir_name, getcwd(buf, BUFSIZ));
		printf("The working directory has been successfully changed to %s.\n",
				dir_name);
	}
}

// 8.
// Recursively lists all files and subdirectories in a given directory and its subdirectories. Print this list to standard output.
void directory_listing_aux(const char *dir_path, int depth) {
	// Open the directory
	DIR *dp = opendir(dir_path);
	if (dp == NULL) {
		printf("Error: cannot open directory");
	}

	// Read the directory entries
	struct dirent *entry;
	while ((entry = readdir(dp)) != NULL) {
		// Skip "." and ".." entries
		if (strcmp(entry->d_name, ".") == 0
				|| strcmp(entry->d_name, "..") == 0) {
			continue;
		}

		// Print the entry with appropriate indentation
		for (int i = 0; i < depth; i++) {
			printf("  ");
		}
		printf("%s\n", entry->d_name);

		// Check if the entry is a subdirectory
		if (entry->d_type == DT_DIR) {
			// Build the full path to the subdirectory
			char sub_dir_path[256];
			snprintf(sub_dir_path, sizeof(sub_dir_path) + 1, "%s/%s", dir_path,
					entry->d_name);

			// Recursively list the subdirectory
			directory_listing_aux(sub_dir_path, depth + 1);
		}
	}

	// Close the directory
	closedir(dp);
}

void directory_listing(const char *start_dir) {

	directory_listing_aux(start_dir, 0);
	printf("%s", start_dir);
}

// 9.
// Removes a given file from the system.
void remove_file(const char *file_name) {

	if (remove(file_name) != 0) {
		printf("Error: cannot remove file");
	} else {
		printf("The %s has been successfully removed:\n", file_name);
	}
}

// 10.
// Removes a given directory and its subdirectories from the system.
// Note: Must prints a message: "Warning: You are about to delete the directory DIRECTORY_NAME and all its contents recursively. Proceed? (y/n)"

void remove_directory_aux(const char *dir_name) {

	// Open the directory
	DIR *dir = opendir(dir_name);
	if (dir == NULL) {
		printf("Error: cannot open directory");
	}

	// Iterate over directory entries
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		// Skip "." and ".." entries
		if (strcmp(entry->d_name, ".") == 0
				|| strcmp(entry->d_name, "..") == 0) {
			continue;
		}

		// Construct the full path to the entry
		char entry_path[256];
		snprintf(entry_path, sizeof(entry_path) + 1, "%s/%s", dir_name,
				entry->d_name);

		// Check if the entry is a subdirectory
		if (entry->d_type == DT_DIR) {
			// Recursively remove subdirectories and their contents
			remove_directory_aux(entry_path);
		} else {
			// Delete regular files
			if (remove(entry_path) != 0) {
				printf("Error: cannot remove file");
			}
		}
	}

	// Close the directory
	closedir(dir);

	// Remove the empty directory
	if (rmdir(dir_name) != 0) {
		printf("Error: cannot remove current directory");
	}
}

void remove_directory(const char *dir_name) {

	printf(
			"Warning: You are about to delete the directory %s and all its contents recursively. Proceed? (y/n) ",
			dir_name);
	char response;
	scanf(" %c", &response);

	if (response == 'y' || response == 'Y') {
		remove_directory_aux(dir_name);
	} else {
		printf("Deletion canceled\n");
	}
}

// -------------------------------TESTING -------------------------------------

int main(void) {

	int ch;
	char sub_dir[100], name[100], file_to_move[100];
	mode_t mode_num = umask(0);
	do {
		printf("\n----- Menu -----:\n"
				"1: Create a directory\n"
				"2: Create and write to a file\n"
				"3: Get Working Directory\n"
				"4: Save /var/log/apt/history.log to a file\n"
				"5: Save /proc/meminfo memory info to a file\n"
				"6: Create a sub-directory and move a file to sub-directory\n"
				"7: Change directory\n"
				"8: List directory contents recursively\n"
				"9: Remove a file\n"
				"10: Remove a Directory\n"
				"99: Quit\n"
				"Enter your choice: %c", ch);

		scanf(" %d", &ch);
		switch (ch) {
		case 1:
			printf("Enter the directory name you want to create: ");
			scanf("%s", name);
			printf("Enter the mode of the created directory: ");
			scanf("%o", &mode_num);
			create_directory(name, umask(mode_num));
			umask(0);
			break;
		case 2:
			printf("Enter the file name you want to change to: ");
			scanf("%s", name);
			create_write_file(name, "Operating System is Fun!!");
			break;
		case 3:
			get_working_dir();
			break;
		case 4:
			printf("Enter the file name you want to store history logs onto: ");
			scanf("%s", name);
			read_historylog_to_file(name);
			break;
		case 5:
			printf(
					"Enter the file name you want to store memory information to: ");
			scanf("%s", name);
			read_proc_mem_info_to_file(name);
			break;
		case 6:
			printf("Enter the sub-directory name you want to create: ");
			scanf("%s", sub_dir);
			printf("Enter the mode of the created directory: ");
			scanf("%o", &mode_num);
			printf(
					"Enter the file name you want to move to the sub-directory: ");
			scanf("%s", file_to_move);
			create_subdirectory_and_move_file(sub_dir, umask(mode_num),
					file_to_move);
			umask(0);
			break;
		case 7:
			printf("Enter the directory name you want to change to: ");
			scanf("%s", name);
			change_directory(name);
			break;
		case 8:
			printf(
					"Enter the directory name you want to list the contents for (if you want to list of the contents of current directory, then pass '.': ");
			scanf("%s", name);
			directory_listing(name);
			break;
		case 9:
			printf("Enter the file name you want to remove: ");
			scanf("%s", name);
			remove_file(name);
			break;
		case 10:
			printf("Enter the directory name you want to remove: ");
			scanf("%s", name);
			remove_directory(name);
			break;
		}
	} while (ch != 99);
	return 0;
}

