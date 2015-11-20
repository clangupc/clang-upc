extern u_intQI_t __getqi3 (long sthread, long saddr);
extern u_intHI_t __gethi3 (long sthread, long saddr);
extern u_intSI_t __getsi3 (long sthread, long saddr);
extern u_intDI_t __getdi3 (long sthread, long saddr);
#if GUPCR_TARGET64
extern u_intTI_t __getti3 (long sthread, long saddr);
#endif /* GUPCR_TARGET64 */
extern float __getsf3 (long sthread, long saddr);
extern double __getdf3 (long sthread, long saddr);
extern long double __gettf3 (long sthread, long saddr);
extern long double __getxf3 (long sthread, long saddr);
extern void __getblk4 (long sthread, long saddr, void *dest, size_t len);
extern void __putqi3 (long dthread, long daddr, u_intQI_t v);
extern void __puthi3 (long dthread, long daddr, u_intHI_t v);
extern void __putsi3 (long dthread, long daddr, u_intSI_t v);
extern void __putdi3 (long dthread, long daddr, u_intDI_t v);
#if GUPCR_TARGET64
extern void __putti3 (long dthread, long daddr, u_intTI_t v);
#endif /* GUPCR_TARGET64 */
extern void __putsf3 (long dthread, long daddr, float v);
extern void __putdf3 (long dthread, long daddr, double v);
extern void __puttf3 (long dthread, long daddr, long double v);
extern void __putxf3 (long dthread, long daddr, long double v);
extern void __putblk4 (const void *src, long dthread, long daddr, size_t n);
/* Strict memory accesses. */
extern u_intQI_t __getsqi3 (long sthread, long saddr);
extern u_intHI_t __getshi3 (long sthread, long saddr);
extern u_intSI_t __getssi3 (long sthread, long saddr);
extern u_intDI_t __getsdi3 (long sthread, long saddr);
#if GUPCR_TARGET64
extern u_intTI_t __getsti3 (long sthread, long saddr);
#endif /* GUPCR_TARGET64 */
extern float __getssf3 (long sthread, long saddr);
extern double __getsdf3 (long sthread, long saddr);
extern long double __getstf3 (long sthread, long saddr);
extern long double __getsxf3 (long sthread, long saddr);
extern void __getsblk4 (long sthread, long saddr, void *dest, size_t len);
extern void __putsqi3 (long dthread, long daddr, u_intQI_t v);
extern void __putshi3 (long dthread, long daddr, u_intHI_t v);
extern void __putssi3 (long dthread, long daddr, u_intSI_t v);
extern void __putsdi3 (long dthread, long daddr, u_intDI_t v);
#if GUPCR_TARGET64
extern void __putsti3 (long dthread, long daddr, u_intTI_t v);
#endif /* GUPCR_TARGET64 */
extern void __putssf3 (long dthread, long daddr, float v);
extern void __putsdf3 (long dthread, long daddr, double v);
extern void __putstf3 (long dthread, long daddr, long double v);
extern void __putsxf3 (long dthread, long daddr, long double v);
extern void __putsblk4 (const void *src, long dthread, long daddr, size_t n);
