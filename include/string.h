
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            string.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef	_TINIX_STRING_H_
#define	_TINIX_STRING_H_

/* PUBLIC */
void*	memcpy(void* p_dst, void* p_src, int size);
void	memset(void* p_dst, char ch, int size);
char*	strcpy(char* p_dst, char* p_src);
int	strlen(char* p_str);

#endif /* _TINIX_STRING_H_ */

