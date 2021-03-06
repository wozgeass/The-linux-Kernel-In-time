#ifndef _M68K_BITOPS_H
#define _M68K_BITOPS_H
/*
 * Copyright 1992, Linus Torvalds.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file README.legal in the main directory of this archive
 * for more details.
 */

/*
 * Require 68020 or better.
 *
 * They use the standard big-endian m680x0 bit ordering.
 */

extern __inline__ int set_bit(int nr,void * vaddr)
{
	char retval;

	__asm__ __volatile__ ("bfset %2@{%1:#1}; sne %0"
	     : "=d" (retval) : "d" (nr^31), "a" (vaddr));

	return retval;
}

extern __inline__ int clear_bit(int nr, void * vaddr)
{
	char retval;

	__asm__ __volatile__ ("bfclr %2@{%1:#1}; sne %0"
	     : "=d" (retval) : "d" (nr^31), "a" (vaddr));

	return retval;
}

extern __inline__ int change_bit(int nr, void * vaddr)
{
	char retval;

	__asm__ __volatile__ ("bfchg %2@{%1:#1}; sne %0"
	     : "=d" (retval) : "d" (nr^31), "a" (vaddr));

	return retval;
}

extern __inline__ int test_bit(int nr, const void * vaddr)
{
	return ((1UL << (nr & 31)) & (((const unsigned int *) vaddr)[nr >> 5])) != 0;
}

extern __inline__ int find_first_zero_bit(void * vaddr, unsigned size)
{
	unsigned long *p = vaddr, *addr = vaddr;
	unsigned long allones = ~0UL;
	int res;
	unsigned long num;

	if (!size)
		return 0;

	while (*p++ == allones)
	{
		if (size <= 32)
			return (p - addr) << 5;
		size -= 32;
	}

	num = ~*--p;
	__asm__ __volatile__ ("bfffo %1{#0,#0},%0"
			      : "=d" (res) : "d" (num & -num));
	return ((p - addr) << 5) + (res ^ 31);
}

extern __inline__ int find_next_zero_bit (void *vaddr, int size,
				      int offset)
{
	unsigned long *addr = vaddr;
	unsigned long *p = addr + (offset >> 5);
	int set = 0, bit = offset & 31UL, res;

	if (offset >= size)
		return size;

	if (bit) {
		unsigned long num = ~*p & (~0UL << bit);

		/* Look for zero in first longword */
		__asm__ __volatile__ ("bfffo %1{#0,#0},%0"
				      : "=d" (res) : "d" (num & -num));
		if (res < 32)
			return (offset & ~31UL) + (res ^ 31);
                set = 32 - bit;
		p++;
	}
	/* No zero yet, search remaining full bytes for a zero */
	res = find_first_zero_bit (p, size - 32 * (p - addr));
	return (offset + set + res);
}

/*
 * ffz = Find First Zero in word. Undefined if no zero exists,
 * so code should check against ~0UL first..
 */
extern __inline__ unsigned long ffz(unsigned long word)
{
	int res;

	__asm__ __volatile__ ("bfffo %1{#0,#0},%0"
			      : "=d" (res) : "d" (~word & -~word));
	return res ^ 31;
}

extern __inline__ int find_first_one_bit(void * vaddr, unsigned size)
{
	unsigned long *p = vaddr, *addr = vaddr;
	int res;
	unsigned long num;

	if (!size)
		return 0;

	while (!*p++)
	{
		if (size <= 32)
			return (p - addr) << 5;
		size -= 32;
	}

	num = *--p;
	__asm__ __volatile__ ("bfffo %1{#0,#0},%0"
			      : "=d" (res) : "d" (num & -num));
	return ((p - addr) << 5) + (res ^ 31);
}

extern __inline__ int find_next_one_bit (void *vaddr, int size,
				      int offset)
{
	unsigned long *addr = vaddr;
	unsigned long *p = addr + (offset >> 5);
	int set = 0, bit = offset & 31UL, res;

	if (offset >= size)
		return size;

	if (bit) {
		unsigned long num = *p & (~0UL << bit);

		/* Look for one in first longword */
		__asm__ __volatile__ ("bfffo %1{#0,#0},%0"
				      : "=d" (res) : "d" (num & -num));
		if (res < 32)
			return (offset & ~31UL) + (res ^ 31);
                set = 32 - bit;
		p++;
	}
	/* No one yet, search remaining full bytes for a one */
	res = find_first_one_bit (p, size - 32 * (p - addr));
	return (offset + set + res);
}

/* Bitmap functions for the minix filesystem */

extern __inline__ int
minix_find_first_zero_bit (const void *vaddr, unsigned size)
{
	const unsigned short *p = vaddr, *addr = vaddr;
	int res;
	unsigned short num;

	if (!size)
		return 0;

	while (*p++ == 0xffff)
	{
		if (size <= 16)
			return (p - addr) << 4;
		size -= 16;
	}

	num = ~*--p;
	__asm__ __volatile__ ("bfffo %1{#16,#16},%0"
			      : "=d" (res) : "d" (num & -num));
	return ((p - addr) << 4) + (res ^ 31);
}

extern __inline__ int
minix_set_bit (int nr, void *vaddr)
{
	char retval;

	__asm__ __volatile__ ("bfset %2{%1:#1}; sne %0"
	     : "=d" (retval) : "d" (nr^15), "m" (*(char *)vaddr));

	return retval;
}

extern __inline__ int
minix_clear_bit (int nr, void *vaddr)
{
	char retval;

	__asm__ __volatile__ ("bfclr %2{%1:#1}; sne %0"
	     : "=d" (retval) : "d" (nr^15), "m" (*(char *) vaddr));

	return retval;
}

extern __inline__ int
minix_test_bit (int nr, const void *vaddr)
{
	return ((1U << (nr & 15)) & (((const unsigned short *) vaddr)[nr >> 4])) != 0;
}

/* Bitmap functions for the ext2 filesystem. */

extern __inline__ int
ext2_set_bit (int nr, void *vaddr)
{
	char retval;

	__asm__ __volatile__ ("bfset %2{%1,#1}; sne %0"
	     : "=d" (retval) : "d" (nr^7), "m" (*(char *) vaddr));

	return retval;
}

extern __inline__ int
ext2_clear_bit (int nr, void *vaddr)
{
	char retval;

	__asm__ __volatile__ ("bfclr %2{%1,#1}; sne %0"
	     : "=d" (retval) : "d" (nr^7), "m" (*(char *) vaddr));

	return retval;
}

extern __inline__ int
ext2_test_bit (int nr, const void *vaddr)
{
	return ((1U << (nr & 7)) & (((const unsigned char *) vaddr)[nr >> 3])) != 0;
}

extern __inline__ int
ext2_find_first_zero_bit (const void *vaddr, unsigned size)
{
	const unsigned long *p = vaddr, *addr = vaddr;
	int res;

	if (!size)
		return 0;

	while (*p++ == ~0UL)
	{
		if (size <= 32)
			return (p - addr) << 5;
		size -= 32;
	}

	--p;
	for (res = 0; res < 32; res++)
		if (!ext2_test_bit (res, p))
			break;
	return (p - addr) * 32 + res;
}

extern __inline__ int
ext2_find_next_zero_bit (const void *vaddr, unsigned size, unsigned offset)
{
	const unsigned long *addr = vaddr;
	const unsigned long *p = addr + (offset >> 5);
	int bit = offset & 31UL, res;

	if (offset >= size)
		return size;

	if (bit) {
		/* Look for zero in first longword */
		for (res = bit; res < 32; res++)
			if (!ext2_test_bit (res, p))
				return (p - addr) * 32 + res;
		p++;
	}
	/* No zero yet, search remaining full bytes for a zero */
	res = ext2_find_first_zero_bit (p, size - 32 * (p - addr));
	return (p - addr) * 32 + res;
}

#endif /* _M68K_BITOPS_H */
