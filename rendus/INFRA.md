# Hackathon - Ynov Toulouse 2025 : Babyfoot du futur - Cloud & Infrastructure

## Equipe

- Cloud & Infra 1 : BOUBEKER Sami
- Cloud & Infra : BESSAIAH Axel

Et si on réinventait l’expérience babyfoot à Ynov ? L’objectif de ce hackathon est de moderniser et digitaliser l’usage des babyfoots présents dans le Souk pour créer un service _next-gen_, pensé pour près de 1000 étudiants !

Que ce soit via des gadgets connectés, un système de réservation intelligent, des statistiques en temps réel ou des fonctionnalités robustes pour une utilisation massive, nous cherchons des solutions innovantes qui allient créativité et technologie.

Toutes les filières sont invitées à contribuer : Dev, Data, Infra, IoT, Systèmes embarqués… chaque idée compte pour rendre le babyfoot plus fun, plus pratique et plus connecté.

Votre mission : transformer le babyfoot classique en expérience high-tech pour Ynov !

---

> Ce fichier contient les informations spécifiques au Cloud & Infra de votre projet. Il suffit d'en remplir une seule fois, même si vous êtes plusieurs Cloud & Infra dans l'équipe.

# Requis

Ce README contient les requis fonctionnels de la partie Cloud & Infra de votre projet. Il doit compléter le README principal à la racine du projet, et servira la partie de votre note propre à votre spécialité.

Basez-vous sur les spécifications dans [SPECIFICATIONS.md](../SPECIFICATIONS.md) pour remplir ce document.

Décrivez ici les actions que vous avez menées, votre démarche, les choix techniques que vous avez faits, les difficultés rencontrées, etc. Précisez également dans quelle mesure vous avez pu collaborer avec les autres spécialités.

Autrement, il n'y a pas de format imposé, mais essayez de rester clair et concis, je ne vous demande pas de rédiger un roman, passez à l'essentiel, et épargnez-moi de longues pages générées par IA (malusée).

En conclusion, cela doit résumer votre travail en tant qu'expert.e infra, et vous permettre de garder un trace écrite de votre contribution au projet.

Merci de votre participation, et bon courage pour la suite du hackathon !

Projet Cloud – Babyfoot

 Compte rendu du projet — Déploiement d’une infrastructure AWS pour l’application Babyfoot App
Contenu du projet
Ce projet avait pour objectif de déployer une infrastructure cloud complète sur AWS pour héberger l’application Babyfoot App, composée :
d’un frontend (interface utilisateur),


d’un backend (API applicative),


et d’une base de données PostgreSQL hébergée sur Amazon RDS.


L’objectif était également d’assurer la sécurité, la scalabilité, et la supervision du système, tout en explorant les outils d’automatisation (Terraform).

















Technologies utilisées
AWS (Amazon Web Services) → hébergement de l’infrastructure :


EC2 : hébergement du backend et du frontend


RDS PostgreSQL : base de données relationnelle


CloudWatch : supervision et alertes


Secrets Manager : stockage sécurisé des identifiants


Terraform → automatisation du déploiement de la base de données


PostgreSQL → gestion des données


Linux (Ubuntu) → environnement des instances EC2


pgAdmin / DBeaver → outils clients pour la gestion de la base de donnée
Architecture
L’architecture est composée de plusieurs services interconnectés :
Utilisateurs
     │
     ▼
 [Frontend - EC2]
     │ (HTTP/HTTPS)
     ▼
 [Backend - EC2]
     │ (connexion PostgreSQL sécurisée)
     ▼
 [Base de données RDS PostgreSQL]
     │
     ├── Sauvegardes automatiques
     └── Journalisation CloudWatch
Chaque composant est hébergé sur AWS :
EC2 (frontend & backend) : serveurs virtuels Linux.


RDS PostgreSQL : instance managée, sauvegardes automatiques et supervision CloudWatch activées.


Sécurité : gestion via Security Groups, IAM Roles et Secrets Manager.


Glossaire : 
Bucket S3 (Amazon Simple Storage Service)
 Un bucket S3 est un conteneur dans lequel tu stockes des objets (fichiers, images, documents, etc.) dans Amazon S3. C’est un service de stockage dans le cloud d’AWS qui permet de stocker et de récupérer des données à tout moment, depuis n'importe où. Les buckets sont organisés par des noms uniques, et tu peux y appliquer des permissions et des politiques de sécurité.


RDS (Relational Database Service)
 RDS est un service géré par Amazon qui facilite la création, la gestion et la mise à l'échelle des bases de données relationnelles dans le cloud. Il prend en charge plusieurs moteurs de bases de données, comme MySQL, PostgreSQL, SQL Server, et MariaDB. RDS s’occupe des tâches de gestion courantes, comme les mises à jour, les sauvegardes, et la gestion des performances.


Cluster
 Un cluster désigne un groupe de serveurs ou d'instances qui travaillent ensemble pour fournir une résilience, une disponibilité élevée, et des performances accrues. Dans AWS, par exemple, un cluster RDS peut être constitué de plusieurs instances de bases de données qui se répliquent entre elles pour assurer la tolérance aux pannes et la disponibilité continue.


IAM (Identity and Access Management)
 IAM est un service AWS qui permet de gérer les utilisateurs, les rôles, et les permissions pour accéder aux ressources AWS. Il définit qui peut accéder à quelles ressources dans AWS, et ce qu’ils peuvent y faire (lire, écrire, administrer, etc.). Grâce à IAM, tu peux appliquer des politiques de sécurité pour contrôler les accès au sein de ton organisation.


PostgreSQL
 PostgreSQL est un système de gestion de bases de données relationnelles (SGBDR) open source très puissant et riche en fonctionnalités. Il est utilisé pour gérer des données structurées avec un langage SQL standard. PostgreSQL est reconnu pour sa conformité aux normes SQL, sa capacité à gérer des transactions complexes, et son extensibilité avec des fonctionnalités comme les types de données personnalisés et les fonctions définies par l'utilisateur.


Terraform
 Terraform est un outil open source développé par HashiCorp qui permet de définir et de gérer l'infrastructure informatique dans le cloud à l'aide de code. Cela permet de décrire l'infrastructure sous forme de fichiers de configuration (Infrastructure as Code) et de la déployer automatiquement sur des services cloud comme AWS, Azure, ou Google Cloud. Terraform gère des ressources telles que des instances de serveurs, des réseaux, des bases de données, etc. Il permet une gestion de l'infrastructure reproductible et versionnée.



Guide de déploiement
Le déploiement a été réalisé en partie avec Terraform et en partie via la console AWS, par manque de temps et d’expérience avec l’automatisation complète.
Étapes principales :
Création de la base de données PostgreSQL (Terraform)


Script Terraform pour créer l’instance RDS (moteur PostgreSQL 17.4).
(voir document word pour toutes les captures d’ecran)


Les identifiants sont stockés de manière sécurisée dans AWS Secrets Manager.


Configuration manuelle via la console AWS


Activation de la journalisation CloudWatch (logs SQL et erreurs).


Mise en place de CloudWatch Alarms (CPU, stockage, connexions).


Création d’un Parameter Group personnalisé (babyfoot-pg-params) avec :


log_min_duration_statement = 2000


rds.force_ssl = 1 (connexion SSL forcée)


Création d’un Security Group spécifique POSTGRESQL autorisant :


Le port 5432/TCP


L’accès depuis l’adresse IP publique de nos collègues afin qu’ils puissent se connecter à distance à la base.


Déploiement des instances EC2


Une instance pour le backend


Une instance pour le frontend


Type : t3 micro (gratuite dans le cadre AWS Free Tier)


Accès SSH sécurisé par clé privée.


Connexion à la base de données


Configuration du client (pgAdmin / DBeaver) avec le point de terminaison RDS.


Connexion SSL activée (sslmode=request).


Tests réalisés avec plusieurs PC ; ajout d’une règle réseau pour autoriser d’autres IPs à accéder à la base.


Scalabilité
L’infrastructure a été pensée pour s’adapter à la charge et permettre une évolution simple à long terme.
Une base de configuration pour un Auto Scaling Group a été préparée afin de gérer automatiquement l’ajout ou la suppression d’instances EC2 selon l’activité (CPU, trafic, etc.).


La base de données RDS dispose de la fonction max_allocated_storage, lui permettant d’augmenter automatiquement sa capacité de stockage en cas de besoin.


Des métriques CloudWatch assurent la surveillance des performances et serviront de base pour automatiser la montée en charge.


Bien que l’autoscaling complet ne soit pas encore actif, l’infrastructure actuelle est déjà compatible avec un environnement élastique et prête à évoluer.


Sécurité
Secrets Manager : stockage du mot de passe de la base de données.


IAM Roles : permissions minimales pour les services EC2.


Security Groups : filtrage strict des ports :


22 (SSH)


80/443 (Web)


5432 (PostgreSQL, restreint à certaines IPs)


Connexion SSL activée entre RDS et les clients PostgreSQL.

On va maintenant créer un rôle IAM (AWS Identity and Access Management) sur notre instance RDS permet :
de donner à ta base RDS des permissions pour accéder à d’autres services AWS (par ex. écrire des logs dans Amazon CloudWatch) ;


ou de permettre à d’autres services AWS de s’authentifier auprès de notre base sans mot de passe (authentification IAM pour Amazon RDS PostgreSQL par exemple).


 Au lieu de distribuer un mot de passe, on relie :
une instance RDS


à un rôle IAM


qui a des politiques de permissions bien définies.


Il permet à une Lambda ou une instance EC2 de se connecter à la base via token IAM sécurisé.
La base peut envoyer ses journaux directement à CloudWatch pour les surveiller.
Le rôle permet à RDS de déposer/restaurer des backups dans Amazon S3.
Pour le créer on va dans IAM → Rôles → Créer un rôle.
Type d’entité de confiance → on choisit RDS (très important).
Cela signifie que ce rôle sera assumé par notre base RDS pour effectuer des actions dans notre compte AWS (par exemple : envoyer des logs dans Amazon CloudWatch).

Sauvegardes et supervision
Sauvegardes automatiques RDS configurées quotidiennement.


Export des logs PostgreSQL vers CloudWatch Logs.


Alertes CloudWatch configurées :


CPU > 80%


FreeStorage < 2 Go


Connexions trop élevées



Sauvegardes automatiques activées
 → Permettent la création d’instantanés réguliers de la base de données.


Période de rétention : 5 jours
 → Chaque sauvegarde est conservée pendant 5 jours avant suppression automatique.


 Fenêtre de sauvegarde planifiée


Heure de début : 01h35 UTC


Durée estimée : 30 minutes


 Réplication inter-régions : non activée
 → Les sauvegardes restent dans la région actuelle, sans copie dans une autre région.


Grâce à cette configuration :
La base de données peut être restaurée à tout moment dans les 5 derniers jours 


Les sauvegardes se font automatiquement chaque nuit 


Cela renforce la sécurité et la résilience sans intervention manuelle 

Configuration de la surveillance RDS
Activation de la surveillance améliorée sur la base de données RDS.
 → Permet de collecter des métriques détaillées, notamment l’utilisation du processeur par thread.


Les données sont envoyées vers CloudWatch Logs avec une granularité de 60 secondes.
 → Un rôle IAM par défaut est utilisé pour cette collecte.


Sélection des types de logs exportés vers CloudWatch :


Erreurs d’authentification IAM


Logs PostgreSQL (requêtes, connexions)


Logs de mise à niveau



Grâce à l’intégration avec CloudWatch, il est possible d’accéder à de nombreuses métriques détaillées, notamment l’utilisation du CPU sur une période donnée.


Ces données permettent de mieux comprendre le comportement de la base et d’anticiper les besoins en ressources.


Elles peuvent également servir de base à une étude de capacité et à la mise en place d’un PCA/PRA (plan de continuité et de reprise d’activité).


Plus le niveau de visibilité est élevé, plus il devient facile d’adapter et d’optimiser l’infrastructure pour garantir disponibilité et performance.


Pour la suite, on autorise notre base de données à interagir avec d’autres services AWS — ici, avec Amazon S3 pour exporter des données. Ce rôle inclut des politiques prédéfinies sécurisées pour que RDS puisse effectuer uniquement les actions nécessaires.
On pourra désormais exporter des snapshots (sauvegardes complètes) de notre base dans un bucket S3 ou lancer des exports en CSV/Parquet si nécessaire et RDS aura le droit d’écrire dans le bucket S3 qu’on aura configuré.
En d’autres termes → la base n’a pas accès à tout S3 → elle a juste le droit d’utiliser S3 pour des opérations d’export.
On peut le voir comme un “dossier géant sécurisé sur Internet” dans lequel on peut stocker et organiser des fichiers.

On va donc par la suite créer le compartiment
→ sur https://s3.console.aws.amazon.com/s3


→ Create bucket


→ On renseigne


Nom du bucket : babyfoot-exports


Région : la même que la base (eu-west-3).
Notre bucket est prêt à recevoir des fichiers.
Une fois notre bucket créé :
Dans RDS → Snapshots


Cliquer sur un snapshot


Cliquer sur Exporter vers S3


Sélectionner le bucket que tu viens de créer (babyfoot-exports)


Lance l’export


Quelques minutes plus tard, on voit dans S3 un fichier contenant ta base ou tes tables.
En résumé :
Un bucket S3 = un espace de stockage dans le cloud


Tu peux l’utiliser pour sauvegarder ou partager des données


Ton rôle IAM s3 export permet à ta base RDS d’y écrire directement


Tu peux gérer les permissions de façon très fine

Instances EC2
Pour les instances EC2 donc les instances qui ont accueilli les serveurs FRONTEND et BACKEND, nous avons fais pareil !
afin de préparer les machines un maximum et donc mis à jour les machines linux UBUNTU
sudo apt update && sudo apt upgrade -y

Ensuite on a implémenté docker via des commandes 
→ sudo docker pull node:22-alpine
puis 
→ node -v
→ npm -v

Hébergement du projet sur AWS
Pour finaliser le déploiement, deux instances EC2 ont été créées sur AWS afin de séparer les rôles applicatifs et d’assurer une meilleure organisation de l’infrastructure. L’instance Backend héberge l’API du projet, qui centralise la logique métier et la communication avec la base de données. Cette API est accessible via son adresse publique et le port configuré dans le groupe de sécurité AWS. L’instance Frontend, quant à elle, héberge le site web permettant aux utilisateurs d’interagir avec la plateforme. Cette séparation entre le front et le back facilite la maintenance, améliore la sécurité et permet une montée en charge plus simple en cas d’augmentation du trafic. Grâce à cette architecture, le projet bénéficie d’un hébergement cloud robuste, scalable et modulaire.
État des lieux
Réalisé :
Déploiement de la base PostgreSQL (Terraform + console AWS)


Configuration de la sécurité (IAM, Security Groups, SSL)


Hébergement du backend et frontend sur EC2


Mise en place de la supervision et des alertes CloudWatch


Règles d’accès réseau pour permettre aux collègues de se connecter à la base


Documentation et test complet de la connexion distante

Non finalisé / Améliorable :
Automatisation complète via Terraform (actuellement, seule la base RDS est gérée automatiquement)


Automatisation du déploiement du frontend/backend (à faire via Terraform ou Ansible et non graphiquement via AWS)


Mise en place d’un DNS personnalisé pour le frontend et le backend


Ajout d’un certificat SSL pour le site web (HTTPS)


Sécurisation plus fine des accès RDS (limiter les IPs au strict nécessaire)

Lien avec les autres pôles du projet
Notre travail d’infrastructure s’intègre directement dans la logique globale du projet Babyfoot connecté, menée en parallèle par les autres pôles :
Pôle FullStack : notre base de données RDS constitue le socle central sur lequel repose leur application web. L’API backend (Node.js / Express) utilise notre base PostgreSQL pour gérer les utilisateurs, les scores et les données de partie.


Pôle IA & Data : les données enregistrées dans notre base sont exploitées pour produire des analyses statistiques et des visualisations, permettant d’identifier le “meilleur joueur”, les heures de pointe ou le babyfoot le plus utilisé.


Pôle IoT / Mobile : les capteurs installés sur les babyfoots envoient leurs données vers l’API, qui les stocke dans notre base hébergée sur AWS. Cette architecture assure une interopérabilité fluide entre les objets connectés, la base de données et les applications front.


Ainsi, notre partie “infrastructure” sert de colonne vertébrale technique au projet : elle relie les données issues des babyfoots (IoT), leur traitement (Data) et leur affichage (FullStack).



Difficultés rencontrées
Au cours du projet, plusieurs difficultés techniques ont freiné la progression :
Problèmes de connexion à la base de données RDS : au départ, les règles de sécurité ne permettaient pas aux autres membres du groupe d’accéder à la base. Après ajustement des Security Groups, le problème a été résolu.


Erreur “no pg_hba.conf entry” : un des postes du groupe ne parvenait pas à se connecter à PostgreSQL. Ce problème provenait de la configuration du réseau et a nécessité la révision des accès autorisés dans AWS.


Blocage du compte AWS : vers la fin du projet, notre compte AWS a été suspendu pour vérification administrative. Ce contrôle automatique, imposé par AWS, a rendu impossible toute création ou modification d’environnement pendant près de 48 heures, mettant temporairement la base de données hors service.


Enfin, nous avons dû faire face à un manque d’expérience sur Terraform, rendant la rédaction des scripts plus lente. La priorité a donc été donnée à la stabilité du déploiement manuel pour respecter les délais du projet.


Malgré ces contraintes, chaque obstacle a été l’occasion de mieux comprendre le fonctionnement d’AWS et de renforcer nos compétences en configuration cloud.

Conclusion :
Ce projet nous a permis de découvrir concrètement le déploiement d’une infrastructure sur AWS, la sécurisation d’une base de données RDS, et l’utilisation d’outils d’automatisation.
 Même si une partie de la configuration a été faite manuellement, nous avons acquis une meilleure compréhension des concepts cloud, de la gestion des permissions, et de la supervision des ressources.


