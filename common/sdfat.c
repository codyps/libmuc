
int sd_putc(int c, FILE *stream)
{
	void *data = fdev_get_udata(stream);
}

int sd_getc(FILE *stream)
{
	void *data = fdev_get_udata(stream);
}

FILE *sd_open(char *path)
{
	FILE *fp = sd_files + pos;
	fdev_setup_stream(fp, sd_putc, sd_getc, _FDEV_SETUP_RW);
	fdev_set_udata(fp, data);
}
