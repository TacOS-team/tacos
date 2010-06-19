#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

void keyboardInterrupt(int id);
size_t write_keyboard(open_file_descriptor *ofd, const void *buf, size_t count);

#endif
