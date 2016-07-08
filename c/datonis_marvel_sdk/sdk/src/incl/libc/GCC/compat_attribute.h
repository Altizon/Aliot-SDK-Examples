


#ifndef _COMPAT_ATTRIBUTE_H_
#define _COMPAT_ATTRIBUTE_H_


#define __deprecated__(x) __attribute__((deprecated(x)))
#define __packed__ __attribute__ ((packed))
#define __unused__ __attribute__((unused))

#endif  /* _COMPAT_ATTRIBUTE_H_ */

