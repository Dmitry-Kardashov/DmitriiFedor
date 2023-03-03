.global htmlFile
.section .rodata.files
htmlFile:
    .incbin "LED-Chess-Board.html"       //В MS Windows указать "LED-Chess-Board.html"
    .byte 0