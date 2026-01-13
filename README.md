# Delivery Rush

**ΑΡΙΣΤΟΤΕΛΕΙΟ ΠΑΝΕΠΙΣΤΗΜΙΟ ΘΕΣΣΑΛΟΝΙΚΗΣ**
**ΤΜΗΜΑ ΗΛΕΚΤΡΟΛΟΓΩΝ ΜΗΧΑΝΙΚΩΝ ΚΑΙ ΜΗΧΑΝΙΚΩΝ ΥΠΟΛΟΓΙΣΤΩΝ**
*Μάθημα:* Δομημένος Προγραμματισμός (004)
*Ακαδημαϊκό Έτος:* 2025-2026

**Συγγραφείς:**
* Αντώνιος Καραφώτης (Α.Ε.Μ.: 11891)
* Νικόλαος Αμοιρίδης (A.E.M.: 11836)

---

## Περιεχόμενα
1. [Άδεια Χρήσης](#1-άδεια-χρήσης)
2. [Περιγραφή Λογισμικού](#2-περιγραφή-λογισμικού)
3. [Περιβάλλον Ανάπτυξης](#3-περιβάλλον-ανάπτυξης)
4. [Οδηγίες Μεταγλώττισης](#4-οδηγίες-μεταγλώττισης-compilation)
5. [Οδηγίες Εγκατάστασης](#5-οδηγίες-εγκατάστασης)
6. [Οδηγίες Εκτέλεσης & Χειρισμού](#6-οδηγίες-εκτέλεσης--χειρισμού)
7. [Παράρτημα: Περιγραφή Συναρτήσεων](#7-παράρτημα-περιγραφή-συναρτήσεων)
8. [Αναφορές & Πηγές](#8-αναφορές--πηγές)

---

## 1. Άδεια Χρήσης

Το παρόν λογισμικό διατίθεται υπό την άδεια **MIT License**. Τα πρωτότυπα οπτικοακουστικά μέσα που δημιουργήθηκαν από τους συγγραφείς διατίθενται υπό την άδεια **Creative Commons Attribution 4.0 (CC BY 4.0)**.

### MIT License
> Copyright (c) 2026 Antonios Karafotis - 11891
> Copyright (c) 2026 Nikolaos Amoiridis - 11836
>
> Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
>
> The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

### AUDIOVISUAL ASSETS LICENSE (CC BY 4.0)
Any original audiovisual material (images, sounds, textures) created by the authors for this project is licensed under the **Creative Commons Attribution 4.0 International License**.

To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

*Note: Third-party assets (e.g., from Raylib examples or royalty-free packs) retain their original licenses.*

### THIRD PARTY ASSETS CREDITS
The following assets were used in this project and are not owned by the authors:

* **Map Image (map.jpg):**
  * Source: [Nintendo Life](https://www.nintendolife.com/news/2014/07/this_retro_city_rampage_map_should_be_handy_if_you_get_lost)
  * License: Used for educational purposes
* **Background Music (background_music.mp3):**
  * Source: [Pixabay](https://pixabay.com/music/video-games-game-gaming-minecraft-background-music-377647/)
  * License: Royalty Free
* **Horn Sound (horn.mp3):**
  * Source: [Pixabay](https://pixabay.com/sound-effects/car-honk-386166/)
  * License: Royalty Free

---

## 2. Περιγραφή Λογισμικού

Το λογισμικό **"Delivery Rush"** αποτελεί ένα παιχνίδι προσομοίωσης διανομής φαγητού, υλοποιημένο στη γλώσσα **C** με χρήση της βιβλιοθήκης **Raylib**.
Ο παίκτης αναλαμβάνει τον ρόλο ενός διανομέα φαγητού σε μια πόλη με έντονη κυκλοφορία. Στόχος είναι η παραλαβή παραγγελιών από εστιατόρια και η έγκαιρη παράδοσή τους σε σπίτια, αποφεύγοντας τα οχήματα (αυτοκίνητα, φορτηγά, περιπολικά).

### Βασικά Χαρακτηριστικά:
* **Menu & Options:** Πλήρες μενού περιήγησης, ρυθμίσεις ανάλυσης οθόνης και έντασης ήχου.
* **Traffic System:** Αυτόνομη κίνηση οχημάτων με σύστημα αποφυγής συγκρούσεων και επαναφοράς πορείας.
* **Gameplay Loop:** Σύστημα οικονομίας (αμοιβές/πρόστιμα) και χρονόμετρο αντίστροφης μέτρησης.
* **Οπτικά Εφέ:** Minimap σε πραγματικό χρόνο, ένδειξη βέλους πλοήγησης, μηνύματα επιτυχίας/αποτυχίας.

---

## 3. Περιβάλλον Ανάπτυξης

Για την ανάπτυξη του λογισμικού χρησιμοποιήθηκαν τα εξής:
* **Λειτουργικό Σύστημα:** Microsoft Windows.
* **Γλώσσα Προγραμματισμού:** C (ISO/IEC 9899 standard).
* **Μεταγλωττιστής:** GCC (MinGW-w64)
* **Βιβλιοθήκες:** Raylib (Γραφικά, Ήχος, Χειρισμός).

---

## 4. Οδηγίες Μεταγλώττισης (Compilation)

Ο πηγαίος κώδικας δομείται γύρω από το αρχείο `main.c`, το οποίο ενσωματώνει τις βοηθητικές βιβλιοθήκες (`helpers.c`, `drawTextures.c`).
Για τη δημιουργία του εκτελέσιμου αρχείου, ανοίξτε τερματικό στον φάκελο του έργου και εκτελέστε την εξής εντολή (για GCC):

`gcc main.c helpers.c drawTextures.c -o DeliveryRush.exe -O2 -Wall -I ./libraries -L ./libraries -lraylib -lopengl32 -lgdi32 -lwinmm`

**Ενδεικτική Δομή Φακέλων:**

    /DeliveryRush
        ├── main.c
        ├── helpers.c
        ├── helpers.h
        ├── drawTextures.c
        ├── drawTextures.h
        ├── LICENSE.txt
        ├── assets/
             ├── map.jpg
             ├── mapWithBorders.png
             ├── background_music.mp3
             └── horn.mp3
        └── libraries/
             ├── raylib.h
             ├── raymath.h
             └── libraylib.a

---

## 5. Οδηγίες Εγκατάστασης

Τοποθετήστε το εκτελέσιμο αρχείο (`DeliveryRush.exe`) στον κεντρικό φάκελο της εφαρμογής.
Βεβαιωθείτε ότι υπάρχει ο υποφάκελος `assets/` και ο υποφάκελος `libraries/` βρίσκονται στον ίδιο χώρο, οι οποίοι πρέπει να περιέχουν αντίστοιχα:

* `map.jpg`: Εικόνα φόντου (χάρτης).
* `mapWithBorders.png`: Εικόνα χάρτη για έλεγχο ορίων (collisions).
* `background_music.mp3`: Μουσική παιχνιδιού.
* `horn.mp3`: Ηχητικό εφέ κόρνας.

και

* `raylib.h`
* `raymath.h`
* `libraylib.a`

---

## 6. Οδηγίες Εκτέλεσης & Χειρισμού

Εκτελέστε το αρχείο `DeliveryRush.exe`. Χρησιμοποιήστε το ποντίκι για πλοήγηση στο Μενού.
Χειρισμός εντός παιχνιδιού (Gameplay):

| Πλήκτρο | Λειτουργία |
| :---: | :--- |
| **W** | Κίνηση Μπροστά |
| **S** | Κίνηση Πίσω |
| **A** | Στροφή Αριστερά |
| **D** | Στροφή Δεξιά |
| **K** | Εμφάνιση/Απόκρυψη πληροφοριών παραγγελίας |
| **F** | Εναλλαγή Πλήρους Οθόνης (Fullscreen) |
| **ESC** | Αίτημα εξόδου (Pause/Exit) |

---

## 7. Παράρτημα: Περιγραφή Συναρτήσεων

Ακολουθεί η τεκμηρίωση των συναρτήσεων που αναπτύχθηκαν, ανά αρχείο πηγαίου κώδικα.

### Αρχείο: `main.c`

* **`main`**
  * *Περιγραφή:* Η κύρια συνάρτηση του προγράμματος. Αρχικοποιεί το παράθυρο, φορτώνει τους πόρους (εικόνες/ήχους) και εκτελεί τον κεντρικό βρόχο (Game Loop) διαχειριζόμενη τις καταστάσεις (Menu, Gameplay, Options).
  * *Παράμετροι:* void
  * *Επιστρέφει:* 0 για επιτυχή τερματισμό (int)

### Αρχείο: `helpers.c` / `helpers.h`

* **`InitMapLocations`**
  * *Περιγραφή:* Σαρώνει την εικόνα του χάρτη pixel-προς-pixel για να εντοπίσει τις τοποθεσίες των εστιατορίων (πράσινα pixels) και των σπιτιών (μπλε pixels).
  * *Παράμετροι:* Εικόνα προς σάρωση (map)
  * *Επιστρέφει:* void

* **`CreateNewOrder`**
  * *Περιγραφή:* Δημιουργεί μια νέα παραγγελία επιλέγοντας τυχαία ένα εστιατόριο για παραλαβή και ένα σπίτι για παράδοση.
  * *Παράμετροι:* Καμία
  * *Επιστρέφει:* Δομή με τα δεδομένα της παραγγελίας (Order)

* **`updateOrder`**
  * *Περιγραφή:* Ελέγχει την πρόοδο της παραγγελίας (παραλαβή/παράδοση), μειώνει τον χρόνο και υπολογίζει την αμοιβή ή το πρόστιμο.
  * *Παράμετροι:* Δείκτες στην τρέχουσα παραγγελία (*currentOrder), στο σκορ (*count), στα χρήματα (*totalMoney), στα σπίτια (*houses), στο είδος του μηνύματος προς προβολή (*message), στην τελευταία αμοιβή (lastReward), τη θέση του παίκτη (bikePos) και τον μετρητή σπιτιών (houseCount)
  * *Επιστρέφει:* void

* **`displayOrderMessage`**
  * *Περιγραφή:* Εμφανίζει στην οθόνη μηνύματα επιτυχίας ή αποτυχίας μετά την ολοκλήρωση μιας παραγγελίας.
  * *Παράμετροι:* Δείκτης στο είδος του μηνύματος προς προβολή (*message) και τελευταία αμοιβή (lastReward)
  * *Επιστρέφει:* void

* **`DrawButton`**
  * *Περιγραφή:* Ζωγραφίζει ένα διαδραστικό κουμπί και ελέγχει αν πατήθηκε από το ποντίκι.
  * *Παράμετροι:* Δείκτη σε κείμενο (*text), ορθογώνιο πλαίσιο (rec), μέγεθος γραμματοσειράς (fontSize), χρώματα (κανονικό (color), hover (hoverColor), κειμένου (textColor))
  * *Επιστρέφει:* true αν πατήθηκε, αλλιώς false (bool)

* **`DrawControlKey`**
  * *Περιγραφή:* Ζωγραφίζει πλήκτρο για τις οδηγίες ελέγχου (controls).
  * *Παράμετροι:* Δείκτη σε χαρακτήρα του πλήκτρου (*key), σε χαρακτήρα λειτουργίας του πλήκτρου (*action) και συντεταγμένες του πλήκτρου (x, y)
  * *Επιστρέφει:* void

* **`mapRandomToVehicleType`**
  * *Περιγραφή:* Επιλέγει τύπο οχήματος (επιβατικό, φορτηγό, περιπολικό).
  * *Παράμετροι:* Τυχαίος αριθμός (int)
  * *Επιστρέφει:* Τύπο οχήματος (TYPE_OF_VEHICLE)

* **`selectColor`**
  * *Περιγραφή:* Επιλέγει τυχαία χρώμα οχήματος.
  * *Παράμετροι:* Τύπος οχήματος (selectedVehicle)
  * *Επιστρέφει:* Χρώμα του οχήματος (Color)

* **`willTouchBorder`**
  * *Περιγραφή:* Ελέγχει αν ένα όχημα βρίσκεται σε όριο του δρόμου.
  * *Παράμετροι:* Εικόνα των ορίων (image) και θέση οχήματος (point)
  * *Επιστρέφει:* true αν βρίσκεται εντός ορίων, αλλιώς false (bool)

* **`getVehicleSize`**
  * *Περιγραφή:* Βρίσκει το μέγεθος ενός οχήματος.
  * *Παράμετροι:* Τύπος οχήματος (type), δείκτες σε διαστάσεις οχήματος (*w, *h)
  * *Επιστρέφει:* void

* **`isVehiclePositionValid`**
  * *Περιγραφή:* Ελέγχει αν η θέση ενός οχήματος είναι επιτρεπτή.
  * *Παράμετροι:* Εικόνα χάρτη με όρια (image), συντεταγμένες οχήματος (px, py), τύπος οχήματος (type) και προσανατολισμός οχήματος (rotation)
  * *Επιστρέφει:* true αν η θέση είναι επιτρεπτή, αλλιώς false (bool)

* **`RenderVehicle`**
  * *Περιγραφή:* Ζωγραφίζει το sprite ενός οχήματος σε σωστό μέγεθος και σωστή θέση.
  * *Παράμετροι:* Δομή οχήματος (v) και τρόπος απεικόνισής τους ανάλογα με το είδος τους (carT, truckT, policeT)
  * *Επιστρέφει:* void

* **`vehicleGenerator`**
  * *Περιγραφή:* Αρχικοποιεί τον πίνακα οχημάτων σε τυχαίες, έγκυρες θέσεις στον χάρτη.
  * *Παράμετροι:* Πλήθος οχημάτων (numOfVehicles), πίνακας οχημάτων (vehicles[]), διαστάσεις χάρτη (mapHeight, map Width), εικόνα χάρτη με όρια (mapWithBorders) και αρχική θέση παίκτη (playerStartPos)
  * *Επιστρέφει:* void

* **`updateTraffic`**
  * *Περιγραφή:* Διαχειρίζεται την κίνηση των οχημάτων.
  * *Παράμετροι:* Δείκτη σε πίνακα οχημάτων (*vehicles), μέγιστος αριθμός οχημάτων (maxVehicles), εικόνα χάρτη με όρια (mapWithBorders) και θέση παίκτη (playerPos)
  * *Επιστρέφει:* void

* **`checkCollisionWithVehicles`**
  * *Περιγραφή:* Ελέγχει αν ο παίκτης συγκρούεται με άλλο όχημα.
  * *Παράμετροι:* ορθογώνιο (hitbox) παίκτη (playerRect), δείκτη σε πίνακα οχημάτων (*vehicles), μέγιστος αριθμός οχημάτων (maxVehicles) και χρήση περιθωρίου (useMargin)
  * *Επιστρέφει:* true αν υπάρχει σύγκρουση, αλλιώς false (bool)

* **`GetRandomValidPosition`**
  * *Περιγραφή:* Βρίσκει μια τυχαία έγκυρη θέση στον χάρτη η οποία χρησιμοποιείται για το respawn του παίκτη.
  * *Παράμετροι:* Εικόνα χάρτη (map), δείκτη σε πίνακα οχημάτων (*vehicles), μέγιστος αριθμός οχημάτων (maxVehicles) και διαστάσεις χάρτη (mapWidth, map Height)
  * *Επιστρέφει:* Νέα θέση του παίκτη (Vector2)

### Αρχείο: `drawTextures.c` / `drawTextures.h`

* **`DrawDeliveryBike`**
  * *Περιγραφή:* Δημιουργεί το sprite της μηχανής.
  * *Παράμετροι:* Στόχος που θα ζωγραφιστεί η μηχανή (target)
  * *Επιστρέφει:* void

* **`PrepareCarTexture`**
  * *Περιγραφή:* Δημιουργεί τα γραφικά για τα αυτοκίνητα.
  * *Παράμετροι:* Στόχος που θα ζωγραφιστεί το αυτοκίνητο (target)
  * *Επιστρέφει:* void

* **`PrepareTruckTexture`**
  * *Περιγραφή:* Δημιουργεί τα γραφικά για τα φορτηγά.
  * *Παράμετροι:* Στόχος που θα ζωγραφιστεί το φορτηγό (target)
  * *Επιστρέφει:* void

* **`PreparePoliceTexture`**
  * *Περιγραφή:* Δημιουργεί τα γραφικά για τα περιπολικά.
  * *Παράμετροι:* Στόχος που θα ζωγραφιστεί το περιπολικό (target)
  * *Επιστρέφει:* void

---

## 8. Αναφορές & Πηγές

Για την ολοκλήρωση της εργασίας χρησιμοποιήθηκαν οι κάτωθι πηγές:

### Βιβλιογραφία & Τεχνική Βοήθεια
* **Raylib Cheatsheet:** Αναφορά για τις συναρτήσεις γραφικών και ήχου.
* **Raymath Library:** Για διανυσματικούς υπολογισμούς (αποστάσεις, γωνίες).

### Πόροι Τρίτων (Third Party Assets)
Οι παρακάτω πόροι χρησιμοποιήθηκαν βάσει των αδειών χρήσης τους (βλ. LICENSE.txt):

* **Χάρτης (Map Image):** Nintendo Life (Retro City Rampage map)
  * Πηγή: [https://www.nintendolife.com/news/2014/07/this_retro_city_rampage_map_should_be_handy_if_you_get_lost](https://www.nintendolife.com/news/2014/07/this_retro_city_rampage_map_should_be_handy_if_you_get_lost)
* **Μουσική (Background Music):** Pixabay (Minecraft Style Music)
  * Πηγή: [https://pixabay.com/music/video-games-game-gaming-minecraft-background-music-377647/](https://pixabay.com/music/video-games-game-gaming-minecraft-background-music-377647/)
* **Ήχος Κόρνας (Horn Sound):** Pixabay
  * Πηγή: [https://pixabay.com/sound-effects/car-honk-386166/](https://pixabay.com/sound-effects/car-honk-386166/)
