### Student: Sindrilaru Catalina-Maria
### Grupa: 322 CA

---

# Tema 4 Client Web. Comuncatie cu REST API

## Organizare

In cadrul temei, am plecat de la laboratorul 9, reutilizand fisierele `buffer.c`,
`helpers.c` si `request.c` cu mici adaugari (functia `compute_delete_request`)
si adaugarea token-ului pentru accesul in biblioteca.

Codul principal se afla in `client.c`, functiile pe care le-am considerat ajutatoare
in `utils.c` si functiile ce definesc actiunile fiecari comenzi pe care clientul o
poate interpreta in `actions.c`.

## Utilizarea bibliotecii de parsare JSON Parson

Am ales sa folosesc aceasta biblioteca, deoarece am folosit limbajul C, iar
aceasta ofera simplitate in lucrul cu obiecte JSON, atat in crearea lor, cat si
in parsarea acestora. Am folosit functii din aceasta biblioteca atunci cand
am vrut sa creez un string in format JSON care sa contina un username si o parola,
pe care le-am trimis ulterior la server pentru inregistrare sau pentru
logare, dar si cand am vrut sa creez un string in format JSON ce contine toate
datele unei carti pe care clientul vrea sa o adauge in biblioteca (titlu, autor, gen,
anul publicatiei etc). Pentru a creea aceste obiecte am folosit functii precum: 

- `json_value_init_object` (cu care am initializat un obiect json)
- `json_value_get_object` (prin care am obtinut obiectul anterior)
- `json_object_set_string` (prin care am setat fiecare camp dorit)
- `json_serialize_to_string_pretty` (prin care am transpus obiectul intr-un fomat human-readable)

Am mai folosit functii din aceasta biblioteca, precum (`basic_extract_json_response`,
`json_parse_string`) atunci cand am vrut sa scot din raspunsul primit de la server un
array de obiecte json (ce contineau informatii despre carti) pe care voiam sa le afisez
in client, sau atunci cand am vrut sa extrag din raspunsul primit de la server informatia
despre o carte. Dupa folosirea acestor functii, a fost mult mai usor de afisat string-ul JSON,
aplicand functia `json_serialize_to_string_pretty`.

## Implementare

Implementarea incepe in fisierul `client.c`, in care am initializat string-uri
pentru `cookie` (in care voi stoca token-ul de conectare al clientului curent),
`jwt_token_library`, in care voi stoca token de acces la biblioteca al utilizatorului curent,
`command` pentru comanda primita de client, dar si `logged_in` care va avea valoarea `true`
daca exista un utilizator conectat sau `false` daca nu exista, `action_auth` care va avea
valoarea `true` daca utilizatorul curent are acces la biblioteca sau `false` daca nu are, precum si
`run` ce va avea valoarea `true` pana cand de la tastatura se va primi comanda
`exit`, cand va deveni `false`, iar programul se va opri.

In interiorul `while`, am deschis conexiunea si am citit primita de la tastatura, tratand un caz
separat pentru fiecare comanda, dar si cazul de comanda invalida, cand se va afisa un mesaj sugestiv.

### Register

Am citit username-ul si parola folosind functia `read_input`, ce afiseaza
prompt-ul specifica, iar apoi citeste in continuare pana la newline.
Daca username-ul sau parola contin spatii, sunt considerate invalide si
se afiseaza o eroare. De asemenea, daca exista deja un utilizator conectat,
se afiseaza o eroare. Pentru a face comunicarea cu serverul, am creat functia
`register_action` in care mai intai am format string-ul in format json
cu username-ul si parola, apoi am compus mesajul de `post` pe care l-am trimis catre server
si de la care am asteptat un raspuns. Avand in vedere ca raspunsul incepe cu
`HTTP/1.1 201 Created` sau cu `HTTP/1.1 400 Bad-Request`, am scos al doilea cuvant din 
raspuns si am verificat daca incepe cu `2`, ceea ce inseamna o inregistrare cu succes,
iar altfel am afisat un mesaj de eroare.

### Login

Actiunea de login se desfasoara in mod asemenator, doar ca in caz de succes
se va salva cookie-ul, pe care l-am gasit cautand in raspuns cuvantul `connect.sid`,
iar variabila `logged_in` va fi setata pe true.

### Enter Library

Am verificat ca exista un utilizator conectat, apoi am trimis un `get request`
catre server, iar in raspuns am cautat cuvantul `token` dupa care urmeaza
token-ul propriu zis pe care l-am extras si salvat. De asemenea am facut
variabila `action_auth` true.

### Get Books

Am verificat ca exista un utilizator conectat si ca are permisiunea de a realiza actiuni 
asupra bibliotecii, apoi am procedat asemenator, doar ca raspunsul de la server l-am pasat
unei functii ce se foloseste de biblioteca parson pentru a scoate array-ul de carti,
verificand mai intai daca este gol sau nu, iar apoi il converteste intr-un format human-readable
si il afiseaza.

### Get Book

Am verificat ca exista un utilizator conectat si ca are permisiunea de a realiza actiuni
asupra bibliotecii, apoi am citit id-ul cartii pentru care se doreste afisarea. Am conecatenat
la path-ul pentru carti, id-ul cartii dorite si am trimis un `get_request` la server,
verificandu-se apoi daca raspunsul primit este un succes sau nu (asa cum am mentionat la register). Daca este
un succes, am pasat raspunsul unei functii ce foloseste functii din biblioteca
parson, la fel ca la get_books, pentru a afisa informatiile despre carte intr-un format json 
human-readable. Altfel, se afiseaza o eroare.

### Add Book

La fel ca anterior, se verifica ca exista un user logat si ca are permisiuni,
apoi, se afiseaza campurile ce au nevoie de completare, salvandu-se raspunsul (prin functia read_input),
verificandu-se daca numarul de pagini a fost introdus in formatul dorit (numar pozitiv). Apoi se 
formeaza un string ce contine datele despre carte in formatul json potrivit, folosind functiile 
din biblioteca parson, si se trimite catre server un `post_request`.

### Delete Book

La fel ca anterior, se verifica ca exista un user logat si ca are permisiuni,
apoi se citeste id-ul carti ce se doreste a fi stearsa, prin care se obtine path-ul potrivit, 
trimitandu-se apoi catre server un `delete_request`.

### Logout

Se trimite un `get_request` catre server, iar daca raspunsul este cu succes,
se seteaza `logged_in` si `action_auth` pe `false` si se sterg cookie-ul
si token-ul de acces in biblioteca.

## Get, Post, Delete Requests

Asa cum am mentionat anterior, functiile de get si post au fost preluate din
laborator, iar cea de delete realizata dupa modelul celei de get.

### Get
Am compus mesajul din `GET %s HTTP/1.1` catre url-ul dat de mine ca parametru,
am adaugat hostul(ip server + port), am adaugat token-ul de acces in biblioteca si cookiul
ce contine token-ul pentru utilizator.

### Post

Am realizat aceleasi etape ca la GET, doar ca am adaugat mesajul ce contine `POST %s HTTP/1.1`
si am trimis si datele date ca parametru de catre client, impreuna cu tipul si lungimea acestui continut.

### Delete

La fel ca la get, doar ca mesajul este `DELETE %s HTTP/1.1` catre url-ul dorit.