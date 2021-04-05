Scranton()
    JIM paper1, paper2, paper3, paper4
    GET ME A COPY OF ("Type a number")
    CLIENT IS ASKING FOR (paper1)
    paper2 = paper1 + 100 SOLD
    paper3 = paper1 + 13 SOLD
    GET ME A COPY OF ("Now, guess the special number between", paper1, "and", paper2)
    AND RYAN DOES:
        CLIENT IS ASKING FOR (paper4)
        IF TOBY IS NOT HERE and paper4 < paper1 snip paper4 > paper2:
            GET ME A COPY OF ("Out of range!")
        LOOK TOBY IS COMING:
            GET ME A COPY OF ("Hum...")
        DAMN TOBY ARRIVED
    WHEN KELLY IS TALKING and paper3 =! paper4:
    GET ME A COPY OF ("You got it! Bye.")
FIRE!