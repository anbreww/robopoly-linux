;******************************************************************************
;                                                                             *
;    Filename: lcam.s                                                         *
;    Date: 23/05/2009                                                         *
;    File Version: 1.3                                                        *
;                                                                             *
;    Authors: Christophe Winter                                               *
;             Fabrizio Lo Conte                                               *
;             Eric Seuret                                                     *
;                                                                             * 
;******************************************************************************
;                                                                             *
; Fonctions assembleur de la librairie C pour la caméra linéaire TSL3301.     *
; Ces fonctions récupère les données et les traites (consultez lcam.h pour    *
; plus d'informations							      *
;                                                                             *
;******************************************************************************

;Modifications
;23/05/2009:	Ajout d'un timeout dans lcam_readout pour pas rester bloqué
;				si la caméra plante (Christophe Winter)
;
;04/05/2007:	Fichier d'origine lcam.asm, modifié par Eric Seuret pour l'interface C




#include <avr/io.h> 


;-----Configuration
.equ 	LCAM_PORT 	, 		_SFR_IO_ADDR(PORTC) 	; Port sur lequel la cam est branchée
.equ 	LCAM_DDR	, 		_SFR_IO_ADDR(DDRC)	; DDR du port sur lequel la cam est branchée
.equ 	LCAM_PIN	,		_SFR_IO_ADDR(PINC)	; Port sur lequel la cam est branchée

.equ 	LCAM_SDIN 	, 		3		; Pin du port sur lequel le SDIN de la cam est branché
.equ	LCAM_SDOUT 	,		4		; Pin du port sur lequel le SDOUT de la cam est branché
.equ 	LCAM_SCLK 	,		5		; Pin du port sur lequel le SDCLK de la cam est branché



;-----Pulse
;Sort un pulse sur le sclk
.macro LPULSE
	sbi		LCAM_PORT, LCAM_SCLK
	nop
	cbi		LCAM_PORT, LCAM_SCLK
.endm


;******************************************************************************
;                                                                             *
;    Filename: lcam.asm                                                       *
;    Date: 14/03/2006                                                         *
;    File Version: 1.1                                                        *
;                                                                             *
;    Authors: Christophe Winter                                               *
;             Fabrizio Lo Conte                                               *
;             Eric Seuret                                                     *
;                                                                             * 
;******************************************************************************
;                                                                             *
; Contient les fonctions de la libraire lcam. Cette libraire permet l'acqui-  *
; sition de donnée par une camera linéaire TSL3301 de TAOS.                   *
;                                                                             *
;******************************************************************************

.global lcam_setup
lcam_setup:
	push	r18

	ldi 	r18, 0x40 				;Left offset
	rcall 	lsend
	ldi	r18, 0
	rcall 	lsend
	ldi 	r18, 0x41 				;Left gain
	rcall 	lsend
	ldi 	r18, 15
	rcall 	lsend
	ldi 	r18, 0x42 				;Middle offset
	rcall 	lsend
	ldi 	r18, 0
	rcall 	lsend
	ldi 	r18, 0x43 				;Middle gain
	rcall 	lsend
	ldi 	r18, 15
	rcall 	lsend
	ldi 	r18, 0x44 				;Right offset
	rcall 	lsend
	ldi 	r18, 0
	rcall 	lsend
	ldi 	r18, 0x45 				;Right gain
	rcall 	lsend
	ldi 	r18, 15
	rcall 	lsend

	pop		r18

	ret


.global lcam_initport
;-----
;Initialisation des port
lcam_initport:
	sbi 	LCAM_DDR, LCAM_SDIN
	sbi 	LCAM_DDR, LCAM_SCLK
	cbi		LCAM_DDR, LCAM_SDOUT
	cbi		LCAM_PORT, LCAM_SCLK 			; Pour en être sur
	ret

;-----Envoi de donnée/instruction
;Envoie à la caméra linéaire le byte stocker dans le registre r18
lsend:
	push	r19
	push	r21

	;Start bit (SDIN à 0)
	cbi 	LCAM_PORT, LCAM_SDIN
	LPULSE

	ldi 	r19, 8					;for 1 to 8
lsend_next:									;envoi des 8 bits (LSB en premier)
	bst		r18, 0
	in		r21, LCAM_PIN
	bld		r21, LCAM_SDIN
	out 	LCAM_PORT, r21			;SDIN = r18.0
	LPULSE
	ror		r18						;r18 rotate right one
	dec		r19
	brne	lsend_next						;endfor

	;Stop bit (SDIN à 1)
	sbi		LCAM_PORT, LCAM_SDIN
	LPULSE

	pop		r21
	pop		r19
	ret

.global lcam_reset
;-----Reset
;Assure que la TSL3301 soit opérationnelle
lcam_reset:
	push	r18

	cbi		LCAM_PORT, LCAM_SCLK

	cbi		LCAM_PORT, LCAM_SDIN			;SDIN = 0

	push	r19						;30 impulsions sur sclk

	ldi		r19, 30					;for 1 to @0
lpulsen_next1:
	sbi		LCAM_PORT, LCAM_SCLK
	dec		r19
	cbi		LCAM_PORT, LCAM_SCLK
	brne	lpulsen_next1					;endfor

	pop		r19

	sbi		LCAM_PORT, LCAM_SDIN			;SDIN = 1

	push	r19						;10 impulsions sur sclk

	ldi		r19, 10					;for 1 to @0
lpulsen_next2:
	sbi		LCAM_PORT, LCAM_SCLK
	dec		r19
	cbi		LCAM_PORT, LCAM_SCLK
	brne	lpulsen_next2					;endfor

	pop		r19


	ldi 	r18, 0x1B
	rcall	lsend							;Commande de Reset

	push	r19						;5 impulsions sur sclk

	ldi		r19, 5					;for 1 to @0
lpulsen_next3:
	sbi		LCAM_PORT, LCAM_SCLK
	dec		r19
	cbi		LCAM_PORT, LCAM_SCLK
	brne	lpulsen_next3					;endfor

	pop		r19

	ldi 	r18, 0X5F
	rcall	lsend							;Ecriture du mode register
	ldi 	r18, 0x00
	rcall	lsend							;Clear mode register(single chip, not sleep)

	pop 	r18
	ret

.global	lcam_startintegration
;-----Intégration
;Demande à la caméra de prendre une image.
lcam_startintegration:
	push	r18

	ldi 	r18,0x08					;Commande STARTInt
	rcall 	lsend

	push	r19						;22 impulsions sur sclk

	ldi		r19, 22					;for 1 to @0
lpulsen_next4:
	sbi		LCAM_PORT, LCAM_SCLK
	dec		r19
	cbi		LCAM_PORT, LCAM_SCLK
	brne	lpulsen_next4					;endfor

	pop		r19


	pop		r18
	ret


.global	lcam_endintegration
;-----Fin d'intégration
;Demande à la caméra de terminer l'intégration une fois le temps d'exposition souhaiter terminé
lcam_endintegration:
	push 	r18

	ldi 	r18,0x10					;Commande SAMPLEInt
	rcall 	lsend

	push	r19						;5 impulsions sur sclk

	ldi		r19, 5					;for 1 to @0
lpulsen_next5:
	sbi		LCAM_PORT, LCAM_SCLK
	dec		r19
	cbi		LCAM_PORT, LCAM_SCLK
	brne	lpulsen_next5					;endfor

	pop		r19


	pop 	r18
	ret

.global lcam_readout
;-----Préparation de l'envoie des donnée
;Donne à la caméra l'odre de se préparer à envyer les donnée et attends qu'elle soit prête
lcam_readout:
	push	r18

	ldi 	r18, 0x02				;Commande READPixel
	rcall 	lsend

	clr		r24		; retourne 0 si pas d'erreur
	clr 	r25
	sbis	LCAM_PIN, LCAM_SDOUT			;Boucle d'attente que la caméra soit prête
	rjmp 	lcam_readout_end

	ldi		r18, 255
lcam_readout_next:
	LPULSE
	;nop
	dec		r18
	breq	lcam_readout_end_err				; timeout si la caméra plante
	sbic	LCAM_PIN, LCAM_SDOUT
	rjmp	lcam_readout_next
	rjmp	lcam_readout_end

lcam_readout_end_err:								; valeur de retour d'erreur 0xFF
	ldi		r24, 0xFF

lcam_readout_end:
	pop		r18
	ret


.global	lcam_read
;-----Lecture des donnée
;Une fois la caméra prête, lit les 102 pixels et les stock en SRAM (addresse lcam_buffer)
lcam_read:
	push	r18
	push	r21
	push	r19
	push	r20
	push	r26
	push	r27

	mov 	r26, r24 					;adresse (low) du buffer
	mov 	r27, r25					;adresse (high) du buffer


	ldi 	r19, 102					;for 1 to 102
lcam_read_nextpixel:
	ldi 	r18, 0					;réception d'un pixel (LSB en premier)

	ldi 	r20, 8					;for 1 to 8
	LPULSE
	clc
lcam_read_nextbit:
	ror 	r18						;Rotate right r18
	in 		r21, LCAM_PIN
	bst		r21, LCAM_SDOUT
	bld		r18, 7					;r18.7 = SDOUT
	LPULSE
	dec 	r20
	brne 	lcam_read_nextbit					;endfor
	LPULSE

	st 		X+, r18					;sauvegarde le pixel
	dec 	r19
	brne 	lcam_read_nextpixel					;endfor

	pop		r27
	pop		r26
	pop		r20
	pop		r19
	pop		r21
	pop		r18
	ret


.global lcam_getpic
;-----Le pic de plus haute valeur
;partage les 102 pixels en 25 zones de 4 pixels (ignorant les pixels extrêmes)
;et retourne sur r0 le numéro de la zone la 
;pluslumineuse (moyenne).
lcam_getpic:
	push	r1
	push	r2
	push	r30
	push	r31
	push 	r0
	push	r26
	push	r27
	push	r28
	push	r29


	mov 	r28, r24	;adresse 
	mov 	r29, r25

	adiw 	r24, 1


	mov 	r26, r24
	mov 	r27, r25



	;Création des groupes
	ldi		r31, 25							;for 1 to 25
lcam_getpic_nextgroup:
	clr		r1

	ldi		r30, 4							;for 1 to 4
lcam_getpic_nextpixel:
	ld		r0, X+

	lsr		r0
	lsr		r0								;division par quatre : deux décalage à droite

	add 	r1, r0							;on le premier quart

	dec		r30
	brne	lcam_getpic_nextpixel				;endfor

	st		y+, r1

	dec		r31
	brne	lcam_getpic_nextgroup


	;Recherche du plus haut groupe
	mov 	r28, r24
	mov 	r29, r25

	clr		r0
	clr		r31								;for 1 to 25
	clr		r2
	ldi		r30, 0xFF

lcam_getpic_nextgroup1:
	inc		r31
	ld		r1, y+
	cp		r1, r2

	brlo	lcam_getpic_smaller
	mov		r0, r31
	mov		r2, r1

lcam_getpic_smaller:
	cp		r1, r30

	brsh	lcam_getpic_none
	mov		r30, r1

lcam_getpic_none:
	cpi		r31, 25
	brne	lcam_getpic_nextgroup1				;endfor

	sub 	r2, r30
	mov		r31, r2
	cpi		r31, 11
	brsh	lcam_getpic_end
	clr		r0

lcam_getpic_end:

	mov		r24, r0
	clr 	r25

	pop		r29
	pop		r28
	pop		r27
	pop		r26
	pop		r0
	pop		r31
	pop		r30
	pop		r2
	pop		r1
	ret






