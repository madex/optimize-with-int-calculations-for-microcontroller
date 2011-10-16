;-----------------------------------------------------------------------------;
; Fast integer squareroot routines for avr-gcc project          (C)ChaN, 2008
;-----------------------------------------------------------------------------;
; uint16_t isqrt32 (uint32_t n);
; uint8_t isqrt16 (uint16_t n);
; uint16_t ihypot (int16_t x, int16_t y);

;-----------------------------------------------------------------------------:
; 32bit integer squareroot
;-----------------------------------------------------------------------------;
;   uint16_t isqrt32 (
;     uint32_t n
;   );
;
; Return Value:
;   Squareroot of n.
;
; Size  = 53 words
; Clock = 532..548 cycles
; Stack = 0 byte

.global isqrt32
.func isqrt32

isqrt32:
	clr	r0
	clr	r18
	clr	r19
	clr	r20
	ldi	r21, 1
	clr	r27
	clr	r30
	clr	r31
	ldi	r26, 16
1:	lsl	r22
	rol	r23
	rol	r24
	rol	r25
	rol	r0
	rol	r18
	rol	r19
	rol	r20
	lsl	r22
	rol	r23
	rol	r24
	rol	r25
	rol	r0
	rol	r18
	rol	r19
	rol	r20
	brpl	2f
	add	r0, r21
	adc	r18, r27
	adc	r19, r30
	adc	r20, r31
	rjmp	3f
2:	sub	r0, r21
	sbc	r18, r27
	sbc	r19, r30
	sbc	r20, r31
3:	lsl	r21
	rol	r27
	rol	r30
	andi	r21, 0b11111000
	ori	r21, 0b00000101
	sbrc	r20, 7
	subi	r21, 2
	dec	r26
	brne	1b
	lsr	r30
	ror	r27
	ror	r21
	lsr	r30
	ror	r27
	ror	r21
	mov	r24, r21
	mov	r25, r27
	ret
.endfunc



;-----------------------------------------------------------------------------:
; 16bit integer squareroot
;-----------------------------------------------------------------------------;
;   uint8_t isqrt16 (
;     uint16_t n
;   );
;
; Return Value:
;   Squareroot of n.
;
; Size  = 33 words
; Clock = 181..189 cycles
; Stack = 0 byte

.global isqrt16
.func isqrt16

isqrt16:
	clr	r18
	clr	r19
	ldi	r20, 1
	clr	r21
	ldi	r22, 8
1:	lsl	r24
	rol	r25
	rol	r18
	rol	r19
	lsl	r24
	rol	r25
	rol	r18
	rol	r19
	brpl	2f
	add	r18, r20
	adc	r19, r21
	rjmp	3f
2:	sub	r18, r20
	sbc	r19, r21
3:	lsl	r20
	rol	r21
	andi	r20, 0b11111000
	ori	r20, 0b00000101
	sbrc	r19, 7
	subi	r20, 2
	dec	r22
	brne	1b
	lsr	r21
	ror	r20
	lsr	r21
	ror	r20
	mov	r24, r20
	ret
.endfunc



;-----------------------------------------------------------------------------:
; 16bit integer hypot (megaAVR is required)
;-----------------------------------------------------------------------------;
;   uint16_t ihypot (
;     int16_t x,
;     int16_t y
;   );
;
; Return Value:
;   Squareroot of (x*x + y*y)
;
; Size  = 42 words
; Clock = 581..597 cycles
; Stack = 0 byte

.global ihypot
.func ihypot

ihypot:
	clr	r26
	sbrs	r25, 7
	rjmp	1f
	com	r24
	com	r25
	adc	r24, r26
	adc	r25, r26
1:	sbrs	r23, 7
	rjmp	2f
	com	r22
	com	r23
	adc	r22, r26
	adc	r23, r26
2:	mul	r22, r22
	movw	r18, r0
	mul	r23, r23
	movw	r20, r0
	mul	r22, r23
	add	r19, r0
	adc	r20, r1
	adc	r21, r26
	add	r19, r0
	adc	r20, r1
	adc	r21, r26
	mul	r24, r24
	movw	r30, r0
	mul	r25, r25
	add	r18, r30
	adc	r19, r31
	adc	r20, r0
	adc	r21, r1
	mul	r24, r25
	add	r19, r0
	adc	r20, r1
	adc	r21, r26
	add	r19, r0
	adc	r20, r1
	adc	r21, r26
	movw	r24, r20
	movw	r22, r18
	clr	r1
	rjmp	isqrt32
.endfunc

;-----------------------------------------------------------------------------:
; 8bit x 8bit unsigned multiply
;
; Register variables:
;  Call:  var10 = 8bit multiplicand
;         var11 = <don't care>
;         var20 = 8bit multiplier
;         lc    = <don't care> (high register must be allocated)
;
;  Result:var1[1:0] = 16bit result of var10 * var20
;         var20     = <not changed>
;         lc        = 0
;
; Size  = 9 words
; Clock = 64 cycles (+ret)
; Stack = 0 byte


mul08u:		sub	var11,var11	;initialize variables
		ldi	lc,9		; lc = 9;
		brcc	PC+2		;---- calcurating loop
		add	var11,var20	;
		ror	var11		;
		ror	var10		;
		dec	lc		;if (--lc > 0)
		brne	PC-5		; continue loop;
		ret



mul08s:		clr	lc		; Optional fast signed helper
		tst	var10		;  process with fast unsinged routine
		brpl	PC+3
		inc	lc
		neg	var10
		tst	var20
		brpl	PC+3
		inc	lc
		neg	var20
		bst	lc,0
		rcall	mul08u
		brtc	PC+5
		com	var10
		com	var11
		adc	var10,lc
		adc	var11,lc
		ret

;-----------------------------------------------------------------------------:
; 16bit x 16bit unsigned multiply
;
; Register Variables
;  Call:  var1[1:0] = 16bit multiplicand
;         var1[3:2] = <don't care>
;         var2[1:0] = 16bit multiplier
;         lc        = <don't care> (high register must be allocated)
;
;  Result:var1[3:0] = 32bit result of var1[1:0] * var2[1:0]
;         var2[1:0] = <not changed>
;         lc        = 0
;
; Size  = 13 words
; Clock = 157..172 cycles (+ret)
; Stack = 0 byte


mul16u:		sub	var13,var13	;initialize variables
		sub	var12,var12	;
		ldi	lc,17		; lc = 17;
		brcc	PC+3		;---- calcurating loop
		add	var12,var20	;
		adc	var13,var21	;
		ror	var13		;
		ror	var12		;
		ror	var11		;
		ror	var10		;
		dec	lc		;if (--lc > 0)
		brne	PC-8		; continue loop;
		ret



mul16s:		clr	lc		; Optional fast signed helper
		clr	var12		;  process with fast unsinged routine
		tst	var11
		brpl	PC+6
		inc	var12
		com	var10
		com	var11
		adc	var10,lc
		adc	var11,lc
		tst	var21
		brpl	PC+6
		inc	var12
		com	var20
		com	var21
		adc	var20,lc
		adc	var21,lc
		bst	var12,0
		rcall	mul16u
		brtc	PC+9
		com	var10
		com	var11
		com	var12
		com	var13
		adc	var10,lc
		adc	var11,lc
		adc	var12,lc
		adc	var13,lc
		ret

;-----------------------------------------------------------------------------;
; 32bit x 32bit unsigned multiply
;
; Register Variables
;  Call:  var1[3:0] = 32bit multiplicand
;         var1[7:4] = <don't care>
;         var2[3:0] = 32bit multiplier
;         lc        = <don't care> (high register must be allocated)
;
;  Result:var1[7:0] = 64bit result of var1[3:0] * var2[3:0]
;         var2[3:0] = <not changed>
;         lc        = 0
;
; Size  = 21 words
; Clock = 436..532 cycles (+ret)
; Stack = 0 byte


mul32u:		sub	var17,var17	;initialize variables
		sub	var16,var16	;
		sub	var15,var15	;
		sub	var14,var14	;
		ldi	lc,33		; lc = 33;
		brcc	PC+5		;---- calcurating loop
		add	var14,var20	;
		adc	var15,var21	;
		adc	var16,var22	;
		adc	var17,var23	;
		ror	var17		;
		ror	var16		;
		ror	var15		;
		ror	var14		;
		ror	var13		;
		ror	var12		;
		ror	var11		;
		ror	var10		;
		dec	lc		;if (--lc > 0)
		brne	PC-14		; continue loop;
		ret



mul32s:		clr	lc		; Optional fast signed helper
		clr	var14		;  process with fast unsinged routine
		tst	var13
		brpl	PC+10
		inc	var14
		com	var10
		com	var11
		com	var12
		com	var13
		adc	var10,lc
		adc	var11,lc
		adc	var12,lc
		adc	var13,lc
		tst	var21
		brpl	PC+10
		inc	var14
		com	var20
		com	var21
		com	var22
		com	var23
		adc	var20,lc
		adc	var21,lc
		adc	var22,lc
		adc	var23,lc
		bst	var14,0

		rcall	mul16u

		brtc	PC+17
		com	var10
		com	var11
		com	var12
		com	var13
		com	var14
		com	var15
		com	var16
		com	var17
		adc	var10,lc
		adc	var11,lc
		adc	var12,lc
		adc	var13,lc
		adc	var14,lc
		adc	var15,lc
		adc	var16,lc
		adc	var17,lc
		ret

;-----------------------------------------------------------------------------:
; 8bit/8bit Unsigned Division
;
; Register Variables
;  Call:  var1 = dividend (0x00..0xff)
;         var2 = divisor (0x01..0x7f)
;         mod  = <don't care>
;         lc   = <don't care> (high register must be allocated)
;
;  Result:var1 = var1 / var2
;         var2 = <not changed>
;         mod  = var1 % var2
;         lc   = 0
;
; Size  = 11 words
; Clock = 66..74 cycles  (+ret)
; Stack = 0 byte

div08u:		clr	mod		;initialize variables
		ldi	lc,8		;  mod = 0; lc = 8;
					;---- calcurating loop
		lsl	var1		;var1 = var1 << 1;
		rol	mod		;mod = mod << 1 + carry;
		cp	mod,var2	;if (mod => var2) {
		brcs	PC+3		; mod -= var2; var1++;
		inc	var1		; }
		sub	mod,var2	;/
		dec	lc		;if (--lc > 0)
		brne	PC-7		; continue loop;
		
;-----------------------------------------------------------------------------:
; 16bit/16bit Unsigned Division
;
; Register Variables
;  Call:  var1[1:0] = dividend (0x0000..0xffff)
;         var2[1:0] = divisor (0x0001..0x7fff)
;         mod[1:0]  = <don't care>
;         lc        = <don't care> (high register must be allocated)
;
;  Result:var1[1:0] = var1[1:0] / var2[1:0]
;         var2[1:0] = <not changed>
;         mod[1:0]  = var1[1:0] % var2[1:0]
;         lc        = 0
;
; Size  = 16 words
; Clock = 179..211 cycles (+ret)
; Stack = 0 byte


div16u:		clr	mod0		;initialize variables
		clr	mod1		;  mod = 0;
		ldi	lc,16		;  lc = 16;
					;---- calcurating loop
		lsl	var10		;var1 = var1 << 1;
		rol	var11		;/
		rol	mod0		;mod = mod << 1 + carry;
		rol	mod1		;/
		cp	mod0,var20	;if (mod => var2) {
		cpc	mod1,var21	; mod -= var2; var1++;
		brcs	PC+4		; }
		inc	var10		;
		sub	mod0,var20	;
		sbc	mod1,var21	;/
		dec	lc		;if (--lc > 0)
		brne	PC-11		; continue loop;
		ret

;-----------------------------------------------------------------------------:
; 24bit/24bit Unsigned Division
;
; Register Variables
;  Call:  var1[2:0] = dividend (0x000000..0xffffff)
;         var2[2:0] = divisor (0x000001..0x7fffff)
;         mod[2:0]  = <don't care>
;         lc        = <don't care> (high register must be allocated)
;
;  Result:var1[2:0] = var1[2:0] / var2[2:0]
;         var2[2:0] = <not changed>
;         mod[2:0]  = var1[2:0] % var2[2:0]
;         lc        = 0
;
; Size  = 21 words
; Clock = 348..412 cycles (+ret)
; Stack = 0 bytes


div24u:		clr	mod0		;initialize variables
		clr	mod1		;  mod = 0;
		clr	mod2		;  lc = 24;
		ldi	lc,24		;/
					;---- calcurating loop
		lsl	var10		;var1 = var1 << 1;
		rol	var11		;
		rol	var12		;/
		rol	mod0		;mod = mod << 1 + carry;
		rol	mod1		;
		rol	mod2		;/
		cp	mod0,var20	;if (mod => var2) {
		cpc	mod1,var21	; mod -= var2; var1++;
		cpc	mod2,var22	; }
		brcs	PC+5		;
		inc	var10		;
		sub	mod0,var20	;
		sbc	mod1,var21	;
		sbc	mod2,var22	;
		dec	lc		;if (--lc > 0)
		brne	PC-15		; continue loop;
		ret

;-----------------------------------------------------------------------------:
; 32bit/32bit Unsigned Division
;
; Register Variables
;  Call:  var1[3:0] = dividend (0x00000000..0xffffffff)
;         var2[3:0] = divisor (0x00000001..0x7fffffff)
;         mod[3:0]  = <don't care>
;         lc        = <don't care> (high register must be allocated)
;
;  Result:var1[3:0] = var1[3:0] / var2[3:0]
;         var2[3:0] = <not changed>
;         mod[3:0]  = var1[3:0] % var2[3:0]
;         lc        = 0
;
; Size  = 26 words
; Clock = 549..677 cycles (+ret)
; Stack = 0 bytes


div32u:		clr	mod0		;initialize variables
		clr	mod1		;  mod = 0;
		clr	mod2		;  lc = 32;
		clr	mod3		;
		ldi	lc,32		;/
					;---- calcurating loop
		lsl	var10		;var1 = var1 << 1;
		rol	var11		;
		rol	var12		;
		rol	var13		;/
		rol	mod0		;mod = mod << 1 + carry;
		rol	mod1		;
		rol	mod2		;
		rol	mod3		;/
		cp	mod0,var20	;if (mod => var2) {
		cpc	mod1,var21	; mod -= var2; var1++;
		cpc	mod2,var22	; }
		cpc	mod3,var23	;
		brcs	PC+6		;
		inc	var10		;
		sub	mod0,var20	;
		sbc	mod1,var21	;
		sbc	mod2,var22	;
		sbc	mod3,var23	;/
		dec	lc		;if (--lc > 0)
		brne	PC-19		; continue loop;
		ret

		

