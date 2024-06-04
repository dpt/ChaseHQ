> $C000 ; ChaseHQ-128K-bank-4.ctl
> $C000 ;
> $C000 ; Bank 4 holds sampled sound data.
> $C000 ;
> $C000
@ $C000 org
b $C000 Sample: "Giddy up boy!"
B $C000,$A8C
b $CA8C Sample: "Let's go Mr. Driver!"
B $CA8C,$14E6
b $DF72 Sample: "Hold on man!"
B $DF72,$A5A
b $E9CC Sample: "Your time's up"
B $E9CC,$ABE
b $F48A Sample: Start noise
B $F48A,$ADC
B $FF66 Looks truncated
u $FF7B Unused
