extern int console_putchar(int c);

int putchar (int c)
{
	return console_putchar(c);
}
