> $C000 ; ChaseHQ-128K-bank-4.skool
> $C000 ;
> $C000 ; Bank 4 holds sampled sound data.
> $C000 ;
> $C000
@ $C000 org
b $C000 Sample: "Giddy up boy!"
B $C000,2700,8*337,4
b $CA8C Sample: "Let's go Mr. Driver!"
B $CA8C,5350,8*668,6
b $DF72 Sample: "Hold on man!"
B $DF72,2650,8*331,2
b $E9CC Sample: "Your time's up"
B $E9CC,2750,8*343,6
b $F48A Sample: Start noise
B $F48A,2780,8*347,4
B $FF66,21,8*2,5 Looks truncated
u $FF7B Unused
B $FF7B,133,8*16,5
