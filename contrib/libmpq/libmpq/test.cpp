#include "mpq.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>

int main(int argc, char **argv)
{
    mpq_archive_s *mpq_archive;
    uint32_t fileCount;
    uint32_t fileIndex = 0;
    libmpq__off_t out_size;
    libmpq__off_t read_size;
    char *out_buf = 0;
    libmpq__off_t count;
    libmpq__off_t i;

    if(argc <= 1 || argc > 3)
    {
        printf("%s%s%s%s%s",
            "libmpq tester\n\n",
            "Usage:\ttest.exe file.mpq [fileIndex]\n",
            "where\n",
            "\tfile.mpq\tname of mpq archive to inspect\n",
            "\tfileIndex\tIndex to file within the mpq archive (optional)\n");

        return 0;
    }

    /* open the mpq archive given as first parameter. */
    libmpq__archive_open(&mpq_archive, argv[1], -1);

    /* get count of files in mpq archive */
    libmpq__archive_files(mpq_archive, &fileCount);
    printf("%u files in %s\n", fileCount, argv[1]);

    if(argc == 3)
    {
        libmpq__off_t offset;
        libmpq__off_t in_size;

        fileIndex = (uint32_t)atoi(argv[2]);

        /* get the offset of file at index */
        libmpq__file_offset(mpq_archive, fileIndex, &offset);
        printf("offset:   %u\n", offset);

        /* get size of file at index */
        libmpq__file_packed_size(mpq_archive, fileIndex, &in_size);
        libmpq__file_unpacked_size(mpq_archive, fileIndex, &out_size);
        printf("packed:   %u\n", in_size);
        printf("unpacked: %u\n", out_size);

        if(out_size > 5000)
        {
            printf("file is large, do you want to see the contents? (y/n)");
            if(getchar() == 'y')
            {
                /* malloc output buffer */
                out_buf = (char*)malloc(out_size);

                /* read, decrypt and unpack file to output buffer. */
                libmpq__file_read(mpq_archive, fileIndex, (uint8_t*)out_buf, out_size, &read_size);

                /* print the buffer to stdout */
                /* files contain \0 all over, so we need to print all characters manually */
                printf("file contents:\n");
                i = 0;
                while( i < read_size)
                {
                    count = printf("%s", &out_buf[i]);
                    i += count <= 0 ? 1 : count;        // incase of \0\0, always increment i by at least one
                }
            }
        }
    }
    else
    {
        if(libmpq__file_number(mpq_archive, "World\\Maps\\Kalimdor\\Kalimdor_50_16.adt", &fileIndex))
		{
			printf("File does not exist.");
			getchar();
			return 0;
		}

        printf("listfile Index: %u\n", fileIndex);

        libmpq__file_unpacked_size(mpq_archive, fileIndex, &out_size);
        printf("listfile size: %u\n", out_size);

        printf("do you want to save the contents of the listfile? (y/n)");
        if(getchar() == 'y')
        {
            out_buf = (char*)malloc(out_size);

            libmpq__file_read(mpq_archive, fileIndex, (uint8_t*)out_buf, out_size, &read_size);

            std::string filename(argv[1]);
            filename += ".listfile";

            FILE* out = fopen(filename.c_str(), "wb");
            fwrite(out_buf, 1, read_size, out);
            fclose(out);
            printf("Listfile written to %s\n", filename.c_str());
        }
    }

    if(out_buf)
        free((void*)out_buf);

    /* close the mpq archive. */
    libmpq__archive_close(mpq_archive);

    return 0;
}
