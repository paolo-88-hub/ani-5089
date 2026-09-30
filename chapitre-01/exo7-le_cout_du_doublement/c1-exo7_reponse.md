# Coût du doublement

## Exemple de résultat

### Résultats

- Nombre de mesures : 1000
- Temps moyen d'un rendu : 4.20 ms
- Estimation du rendu deux fois : 8.40 ms
- Temps restant sur 11 ms : 2.60 ms

### Tableau pour le compte rendu

| Élément | Résultat |
| --- | --- |
| Temps moyen d'un rendu | 4,20 ms |
| Rendu effectué deux fois | 8,40 ms |
| Budget total | 11 ms |
| Temps restant | 2,60 ms |
| À réduire en priorité | Le coût du rendu |

## Conclusion

Il reste 2,60 ms pour le reste du programme.

Le rendu seul prend environ 4,20 ms par image dans notre mesure. Effectué deux fois, il coûterait environ 8,40 ms, ce qui laisserait seulement 2,60 ms pour le reste du programme sur un budget de 11 ms. Il serait donc nécessaire de réduire le coût du rendu ou d'éviter de le réaliser deux fois lorsque cela est possible.
