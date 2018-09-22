
#include "./sprint128x128_16bit.c"

BSP_VOID tft_test_image_white()
{
    char tmp[BUFFER_SIZE] = {0};

    memset(tmp, 0xff, BUFFER_SIZE);
    tft_write_data image;
    image.x_start  = 0;
    image.y_start  = 0;
    image.x_offset = 128;
    image.y_offset = 64;
    image.image = tmp;
    balong_tft_ioctl(NULL, OLED_WRITE_IMAGE, (unsigned long)&image);
}

BSP_VOID tft_test_poweron()
{
    balong_tft_ioctl(NULL, OLED_POWER_ON, NULL);
}

BSP_VOID tft_test_poweroff()
{
    balong_tft_ioctl(NULL, OLED_POWER_OFF, NULL);
}

BSP_VOID tft_show_sprint_16bit(BSP_VOID) 
{
    tftRefresh(0,0,128,128,gImage_sprint128x128_16bit);    
}

