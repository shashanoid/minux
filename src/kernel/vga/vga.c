#include <vga/vga.h>
#include <util/io.h>
#include <std/types.h>
#include <std/builtins.h>
/* fb_get_pos:
 * Get the framebuffer memory positions
 * @param x The x coordinate.
 * @param y The y coordinate
*/
uintptr_t fb_get_pos(uint8_t x, uint8_t y)
{
    return (x + VGA_SCREEN_WIDTH * y) * 2; //Need * 2 for color info and character
}


/* fb_write_string:
 * Writes a string with a given foreground and background.
 *  @param char* string The string to write
 *  @param fg The foreground color
 *  @param bg The background color
*/  
void fb_write_string(uint8_t x, uint8_t y, char* string, uint8_t fg, uint8_t bg)
{
    uint8_t i = 0;
    uint8_t start_pos = fb_get_pos(x, y);
    while(string[i] != 0) {
       fb_write_cell(start_pos + i * 2, string[i], fg, bg);
       i++;
   }
}

/** fb_write_cell:
 *  Writes a character with the given foreground and background to position i
 *  in the framebuffer.
 *
 *  @param i  The location in the framebuffer
 *  @param c  The character
 *  @param fg The foreground color
 *  @param bg The background color
 */
void fb_write_cell(uint32_t i, char c, uint8_t fg, uint8_t bg)
{
    char* fb = (char *)VGA_FB_ADDR;
    fb[i] = c;
    fb[i + 1] = ((bg & 0x0F) << 4) | (fg & 0x0F);
}

/* fb_clear_buffer
 * Clears buffer one character at a time
 * @param bg The background color to use
*/
void fb_clear_buffer(uint8_t fg, uint8_t bg)
{
    for(int y = 0; y < VGA_SCREEN_HEIGHT; y++) {
        for(int x = 0; x < VGA_SCREEN_WIDTH; x++) {
            fb_write_cell(fb_get_pos(x,y), ' ', fg, bg);
        }
    }
}

/* fb_move_cursor
 * Moves the cursor to a specified position.
 * @param x The x position
 * @param y The y position
 */
void fb_move_cursor(uint8_t x, uint8_t y) {
    uint16_t pos = fb_get_pos(x, y)/2;
    outb(VGA_COMMAND_PORT, VGA_HI_BYTE_COM);
    outb(VGA_DATA_PORT, (unsigned char) ((pos >> 8) & 0xFF));
    outb(VGA_COMMAND_PORT, VGA_LO_BYTE_COM);
    outb(VGA_DATA_PORT, (unsigned char) (pos & 0xFF));
}

/* fb_move_cursor
 * Disables the cursor
 */
void fb_disable_cursor() {
    outb(VGA_COMMAND_PORT, 0xA);
    outb(VGA_DATA_PORT, 0x20);
}

void fb_enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
	outb(VGA_COMMAND_PORT, 0x0A);
	outb(VGA_DATA_PORT, (inb(0x3D5) & 0xC0) | cursor_start);
 
	outb(VGA_COMMAND_PORT, 0x0B);
	outb(VGA_DATA_PORT, (inb(0x3E0) & 0xE0) | cursor_end);
}

void fb_scroll(uint8_t fg, uint8_t bg) {
    //Copy up
    memcpy((void *)VGA_FB_ADDR, (void *) VGA_FB_ADDR + fb_get_pos(0, 1), (VGA_SCREEN_HEIGHT - 1) * VGA_SCREEN_WIDTH * 2);
    for(int x = 0; x < VGA_SCREEN_WIDTH; x++) {
        fb_write_cell(fb_get_pos(x, VGA_SCREEN_HEIGHT - 1), ' ', fg, bg);
    }
}