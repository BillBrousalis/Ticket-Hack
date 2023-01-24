# -Work in progress-
# Free transportation for the people

Cloning disposable Mifare Ultralight EV1 tickets, using UID re-writtable cards.

Dump card memory - clone.

TODO:

```
* Waiting for UID-rewritable card to arrive.
* Do not ruin the card by writing somewhere I should not.
```

Notes:

```
-- Response codes

Ah     --> Acknowledge (ACK)
0h     --> NAK for invalid argument (i.e. invalid page address)
1h     --> NAK for parity or CRC error
4h     --> NAK for counter overflow
5h, 7h --> NAK for EEPROM write error
6h, 9h --> NAK, other error
```
