## How to get logical block addresses

```
$$ sudo hdparm --fibmap License.txt 

License.txt:
 filesystem blocksize 4096, begins at LBA 2048; assuming 512 byte sectors.
 byte_offset  begin_LBA    end_LBA    sectors
           0  440751488  440751551         64
```

## How to read from logical block addresses

```
$$ sudo hdparm --read-sector 440751488 /dev/sda

/dev/sda:
reading sector 440751488: succeeded
2020 2020 2020 2020 2020 2020 2020 2020
2020 2020 474e 5520 4745 4e45 5241 4c20
...
```

### Potentially better approach using `dd` (can redirect stdout)
```
$$ sudo dd if=/dev/sda count=1 iflag=direct skip=440751488
<file data>
```
- If we have a filesize then we can just read that many bytes. Probably ideal.