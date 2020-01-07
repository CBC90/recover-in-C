/* Recovers a series of JPEG images */
#include <stdio.h>
#include <stdlib.h>
#include <cs50.h>
#include <math.h>

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./recover image\n");
        return 1;
    }

    // open input file
    char *infile = argv[1];

    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", argv[1]);
        return 2;
    }

    // jpg file count
    int k = 0;

    // open first output file
    char filename[8];

    sprintf(filename, "%03i.jpg", k);

    FILE *img = fopen(filename, "w");
    if (img == NULL)
    {
        fclose(img);
        fprintf(stderr, "Could not create %s.\n", filename);
        return 3;
    }

    // determine amount of blocks (512B) in file
    fseek(inptr, 0, SEEK_END);
    int fn = (ftell(inptr)) / 512;
    fseek(inptr, 0, SEEK_SET);

    // block count
    int i = 0;

    // create an array to save the first 4 bytes of each block
    unsigned char id[4];

    // find first JPEG block
    while (id[0] != 0xff || id[1] != 0xd8 || id[2] != 0xff || ((id[3] & 0xf0) != 0xe0))
    {

        for (int m = 0; m < 4; m++)
        {
            id[m] = fgetc(inptr);
            if (feof(inptr))
            {
                break;
            }
        }

        i++;

        fseek(inptr, 512 * i, SEEK_SET);

        if (i >= fn)
        {
            fprintf(stderr, "No JPEG file found\n");
            return 4;
        }
    }

    i--;

    fseek(inptr, 512 * i, SEEK_SET);

    unsigned char buffer[512];

    // Prints block to JPEG files
    while (i < (int)fn)
    {

        // read first 512 bytes from infile into buffer
        //fread(&buffer, 1, 512, inptr);
        for (int m = 0; m < 512; m++)
        {
            buffer[m] = fgetc(inptr);
            if (feof(inptr))
            {
                break;
            }
        }
        i++;

        // write block into img file
        fwrite(&buffer, 1, 512, img);

        for (int j = 0; j < 4; j++)
        {
            id[j] = fgetc(inptr);
        }

        fseek(inptr, -4, SEEK_CUR);


        if (id[0] == 0xff && id[1] == 0xd8 && id[2] == 0xff && (id[3] & 0xf0) == 0xe0)
        {
            // close outfile k
            fclose(img);

            k++;

            // open new outfile k
            sprintf(filename, "%03i.jpg", k);
            img = fopen(filename, "w");

        }

    }

    fclose(inptr);
    fclose(img);

}
