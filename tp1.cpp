
#include <QDebug>
#include <QString>
#include <QHash>

using Quantity = int;

struct Entity { // décrit tout type d'objet dans le jeu
    QString uid; // code unique
    QString title; // nom d'affichage
};

struct Schema { // décrit comment crafter certains objets
    Entity * obj; // objet à crafter
    QHash<Entity *, Quantity> res; // ressources/objets nécessaires, avec leur quantité respectives
};

struct Player {
    QHash<Entity *, Quantity> inventory; // inventaire du joueur avec les quantité pour chaque objet (des stacks)

    void craft (const Schema & schema) { // fonction pour crafter un objet dans l'inventaire à partir de son schéma
        qInfo () << "----------" << "joueur veut craft" << schema.obj->title << "----------";
        bool error { false };
        for (QHash<Entity *, Quantity>::const_iterator it { schema.res.constBegin () }, end { schema.res.constEnd () }; it != end; ++it) {
            const Quantity tmp { (inventory.value (it.key (), 0) - it.value ()) };
            if (tmp < 0) {
                error = true;
                qWarning () << ">>> Missing" << it.key ()->title << "=" << tmp;
            }
        }
        if (!error) {
            for (QHash<Entity *, Quantity>::const_iterator it { schema.res.constBegin () }, end { schema.res.constEnd () }; it != end; ++it) {
                offsetQuantity (it.key (), -it.value ());
            }
            offsetQuantity (schema.obj, +1);
            qInfo () << ">>> OK";
            list ();
        }
    }

    void farm (Entity * entity, const Quantity number) { // fonction qu'on appelle lorsqu'on farme/ramasse des ressources/objets
        qInfo () << "----------" << "joueur vient de farm" << entity->title << "----------";
        offsetQuantity (entity, +number);
        qInfo () << ">>> +" << number;
    }

    void list (void) { // affichage le contenu de l'inventaire dans le terminal
        for (QHash<Entity *, Quantity>::const_iterator it { inventory.constBegin () }, end { inventory.constEnd () }; it != end; ++it) {
            if (it.value () > 0) {
                qInfo () << it.key ()->title << "=" << it.value ();
            }
        }
    }

private:
    void offsetQuantity (Entity * entity, const Quantity offset) { // fonction qu'on utilise pour modifier la quantité dans la hash
        inventory.insert (entity, (inventory.value (entity, 0) + offset));
    }
};

int main (int argc, char * argv []) {
    Q_UNUSED (argc)
    Q_UNUSED (argv)

    // déclare les entités supportés par le jeu :
    Entity WOOD  { "WOOD",  "Bois"   };
    Entity STONE { "STONE", "Pierre" };
    Entity METAL { "METAL", "Métal"  };
    Entity AXE   { "AXE",   "Hache"  };
    Entity WALL  { "WALL",  "Mur"    };
    Entity HOUSE { "HOUSE", "Maison" };

    // déclare les schémas pour crafter certains objets :
    Schema axe {
        &AXE, {
            { &WOOD,  5 },
            { &METAL, 2 },
        }
    };
    Schema wall {
        &WALL, {
            { &WOOD,   50 },
            { &STONE, 150 },
        }
    };
    Schema house {
        &HOUSE, {
            { &WALL, 4 },
            { &AXE,  1 },
        }
    };

    // initialise un joueur avec un inventaire déjà rempli de ressources de base :
    Player me {
        {
           { &WOOD,  250 },
           { &STONE, 200 },
           { &METAL,  50 },
        }
    };

    // on joue !
    me.list  ();

    me.craft (axe);
    me.craft (wall);
    me.craft (house);
    me.craft (wall);

    me.farm  (&WOOD, 100);
    me.craft (wall);

    me.farm  (&STONE, 50);
    me.craft (wall);

    me.farm  (&STONE, 50);

    me.craft (wall);

    me.craft (house);

    return 0;
}
