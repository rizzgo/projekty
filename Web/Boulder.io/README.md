# Boulder.io

<div id="wrapper">
    <p>
    Boulder.io je webová aplikácia na vytváranie a zdeľanie lezeckých ciest. Primárne rieši problém vymýšľania vlastných ciest na stenách bez ich explicintého vyznačenia. Bola vytvorená pre predmet Tvorba používateľských rozhraní, ako skupinový projekt trojíc.
    </p>
    <p>
    Prezeranie a filtrovanie ciest pre inšpiráciu pri lezení je v aplikácii možné aj bez prihlásenia. Po prihlásení je možné pridávať príspevky s vymyslenými cestami, a tiež hodnotiť a komentovať príspevky ostatných používateľov.
    </p>
    <h2>Ukážky</h2>
    <div id="screenshots">
        <figure class="screenshot">
            <img src="screenshots/1_login.png" alt="1_login_home">
            <figcaption>1. Prihlásenie</figcaption>
        </figure>
        <figure class="screenshot">
            <img src="screenshots/2_registration.png" alt="1_login_home">
            <figcaption>2. Registrácia</figcaption>
        </figure>
        <figure class="screenshot">
            <img src="screenshots/3_editor.png" alt="1_login_home">
            <figcaption>3. Editor fotiek</figcaption>
        </figure>
        <figure class="screenshot">
            <img src="screenshots/4_add_route.png" alt="1_login_home">
            <figcaption>4. Pridanie príspevku</figcaption>
        </figure>
        <figure class="screenshot">
            <img src="screenshots/5_edit_user.png" alt="1_login_home">
            <figcaption>5. Úprava používateľských údajov</figcaption>
        </figure>
        <figure class="screenshot">
            <img src="screenshots/6_user.png" alt="1_login_home">
            <figcaption>6. Používateľský profil</figcaption>
        </figure>
        <figure class="screenshot">
            <img src="screenshots/7_home.png" alt="1_login_home">
            <figcaption>7. Domovská stránka</figcaption>
        </figure>
        <figure class="screenshot">
            <img src="screenshots/8_comment.png" alt="1_login_home">
            <figcaption>8. Pridanie komentára</figcaption>
        </figure>
        <figure class="screenshot">
            <img src="screenshots/9_filter.png" alt="1_login_home">
            <figcaption>9. Filtrovanie príspevkov</figcaption>
        </figure>
    </div>
</div>

<style>
    #screenshots {
        display: grid;
        grid-gap: 50px 80px;
        grid-auto-flow: dense;
        grid-template-columns: repeat(auto-fit, minmax(350px, 1fr));
        max-width: 1400px;
        margin: 20px 0;
    }

    .screenshot {
        margin: 0;
    }

    .screenshot > figcaption {
        padding-top: 5px;
        text-align: center;
    }
</style>   