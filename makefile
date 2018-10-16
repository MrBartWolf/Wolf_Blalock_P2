make:
	gcc parent.c wrappers.c -o parent -pthread
	gcc fline.c wrappers.c -o fline -pthread
	gcc supervisor.c wrappers.c -o supervisor -pthread
