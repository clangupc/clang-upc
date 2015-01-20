extern u_intQI_t __getqi3 (long thread, size_t offset);
extern u_intHI_t __gethi3 (long thread, size_t offset);
extern u_intSI_t __getsi3 (long thread, size_t offset);
extern u_intDI_t __getdi3 (long thread, size_t offset);
#if GUPCR_TARGET64
extern u_intTI_t __getti3 (long thread, size_t offset);
#endif /* GUPCR_TARGET64 */
extern float __getsf3 (long thread, size_t offset);
extern double __getdf3 (long thread, size_t offset);
extern void __getblk4 (long thread, size_t offset, void *dest, size_t n);
extern void __putqi3 (long thread, size_t offset, u_intQI_t v);
extern void __puthi3 (long thread, size_t offset, u_intHI_t v);
extern void __putsi3 (long thread, size_t offset, u_intSI_t v);
extern void __putdi3 (long thread, size_t offset, u_intDI_t v);
#if GUPCR_TARGET64
extern void __putti3 (long thread, size_t offset, u_intTI_t v);
#endif /* GUPCR_TARGET64 */
extern void __putsf3 (long thread, size_t offset, float v);
extern void __putdf3 (long thread, size_t offset, double v);
extern void __putblk4 (void *src, long thread, size_t offset, size_t n);
extern void __copyblk5 (long dthread, size_t doffset,
			long sthread, size_t soffset, size_t n);
extern u_intQI_t __getsqi3 (long thread, size_t offset);
extern u_intHI_t __getshi3 (long thread, size_t offset);
extern u_intSI_t __getssi3 (long thread, size_t offset);
extern u_intDI_t __getsdi3 (long thread, size_t offset);
#if GUPCR_TARGET64
extern u_intTI_t __getsti3 (long thread, size_t offset);
#endif /* GUPCR_TARGET64 */
extern float __getssf3 (long thread, size_t offset);
extern double __getsdf3 (long thread, size_t offset);
extern void __getsblk4 (long thread, size_t offset, void *dest, size_t n);
extern void __putsqi3 (long thread, size_t offset, u_intQI_t v);
extern void __putshi3 (long thread, size_t offset, u_intHI_t v);
extern void __putssi3 (long thread, size_t offset, u_intSI_t v);
extern void __putsdi3 (long thread, size_t offset, u_intDI_t v);
#if GUPCR_TARGET64
extern void __putsti3 (long thread, size_t offset, u_intTI_t v);
#endif /* GUPCR_TARGET64 */
extern void __putssf3 (long thread, size_t offset, float v);
extern void __putsdf3 (long thread, size_t offset, double v);
extern void __putsblk4 (void *src, long thread, size_t offset, size_t n);
extern void __copysblk5 (long dthread, size_t doffset,
			 long sthread, size_t soffset, size_t n);
