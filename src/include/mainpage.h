/** @mainpage Klient server aplikacia editoru a simulatoru vysokourovnovych Petriho sieti.
 * \file mainpage.h
 * \brief Stranky dokumentacie.
 *
 * @authors Fridolin Pokorny fridex.devel@gmail.com
 * @authors Miroslav Lisik xlisik00@stud.fit.vutbr.cz
 *
 * @section README
 * Informacie spojene s prekladom a spustenim su umiestnene v subore README.txt.
 * <hr>
 *
 * @page protokol Protokol klient - server

Projekt vyuziva vlastny protokol pre komunikaciu medzi serverom a klientom.
Je inspirovany protokolom HTTP - spravy sa prenasaju v textovej podobe,
jednotlive polozky su riadkovo orientovane. Pri kazdej poziadavke na server,
sa musi uzivatel autentizovat - polozka PN je teda povinna pri kazdej
poziadavke od klienta ako aj polozka PASS pre uvedenie hesla, heslo je uvadzane v
otvorenej podobe. Protokol je bezstavovy, je teda potrebne zadavat heslo a meno
pre identifikaciu v kazdej poziadavke. Na poradi jednotlivych riadkov pri
dotazovani na server
nezalezi, avsak polozka `XML:' musi byt vzdy uvedena ako posledna (vid nizsie).
Sprava je ukoncena prazdnym riadkom. Riadky su zalamovane pomocou CRLF, pre
komunikaciu so serverom je teda mozne pouzit aplikaciu telnet.

Server odpoveda na kazdu poziadavku klienta, pricom su definovane dva typy
odpovedi. Standardna odpoved informuje o uspesnosti/neuspesnosti prevedenia
poziadavku alebo o chybnom dotazovani. V pripade specifickych dotazov na
server, existuju nestandardne odpovede, ktore server posiela.

Spracovanie poziadavku na strane serveru je implementovane pomocou objektu
Message v subore message.cpp. Zasielanie odpovedi je implemntovane pomocou
objektu Answer v answer.cpp. Na strane klienta je zasielanie a spracovanie
odpovedi od serveru implementovane v subore connection.cpp (trieda
Connection).

* @section protoexample Dotazy na server a ich odpovede
* @subsection auth Autorizacia

Klient: <br>
autentifikacia:
<pre>
    PN: [username]
    DO: AUTH
    PASS: [password]
</pre>
odhlasenie:
<pre>
    PN: [username]
    PASS: [password]
    DO: LOGOUT
</pre>
zaregistrovanie:
<pre>
    PN: [username]
    PASS: [password]
    DO: REG
</pre>

Server:
    Na tieto poziadavky su zasielane standardne odpovede.

* @subsection list Prevzatie zoznamu projektov:
Klient:
<pre>
    PN: [username]
    PASS: [password]
    DO: LIST
</pre>

Server:
<pre>
    LIST:
    NAME: [projectname]
    NAME: [projectname]
</pre>
Pre kazdy projekt NAME, v pripade nenajdenia dotazovaneho
projektu, generuje sa standardna odpoved.

* @subsection list Prevzatie zoznamu projektov:
Klient:
<pre>
    PN: [username]
    PASS: [password]
    DO: VLIST
</pre>

Server:
<pre>
    VLIST:
    DESC: [desc]
    USER: [username]
    DESC: [desc]
    USER: [username]
</pre>
Pre kazdy projekt USER a DESC osobitne, v pripade nenajdenia dotazovaneho
projektu, generuje sa standardna odpoved. DESC reprezentuje popis verzie, USER
reprezentuje uzivatela, ktory verziu ulozil. Zasielane polozky su v poradi od
najstarsej verzie k najnovsej.

 * @subsection get Prevzatie projektu:
Klient:
<pre>
    PN: [username]
    PASS: [password]
    DO: GET
    NAME: [projectname]
    VERSION: [version]
</pre>

Server:
<pre>
    XML:
    &lt;xml/&gt;
</pre>

V pripade nenajdenia dotazovaneho projektu, generuje sa standardna chybova
odpoved.

 * @subsection add Ulozenie projektu:

Klient:
<pre>
    PN: [username]
    PASS: [password]
    DO: ADD
    NAME: [projectname]
    DESC: [desc]
    XML:
    &lt;xml/&gt;
</pre>

* Server:
<pre>
    ADD: [SPRAVA]
    VERSION: [version]
</pre>

 * @subsection step Krokovanie simulacie


Klient:
<pre>
    PN: [username]
    PASS: [password]
    DO: STEP
    XML:
    &lt;xml/&gt;
</pre>

Server:
<pre>
    XML:
    &lt;xml/&gt;
</pre>

 * @subsection run Odsimulovanie

Klient:
<pre>
    PN: [username]
    PASS: [password]
    DO: RUN
    XML:
    &lt;xml/&gt;
</pre>

Server:
<pre>
    XML:
    &lt;xml/&gt;
</pre>

* @subsection simlog Spristupnenie logu simulacie

Klient:
<pre>
    PN: [username]
    PASS: [password]
    DO: SIMLOG
    NAME: [projectname]
    VERSION: [version]
</pre>

Server:
<pre>
    SIMLOG:
    TIME: [time]
    USER: [username]
</pre>

Pre kazdy zaznam v logu je time a user samostatne. Cas je udavany v Unix
timestamp.

 * @section standard Standardne odpovede
Standardne odpovede sa vytvaraju pomocou triedy Answer, ich mozne spravy su
umiestnene v answer.cpp.

Zly poziadavok:
<pre>
    DO: BAD
    MSG: [CHYBOVA HLASKA]
</pre>

Vsetko prebehlo spravne:
<pre>
    DO: OK
    MSG: [SPRAVA]
</pre>

*/
