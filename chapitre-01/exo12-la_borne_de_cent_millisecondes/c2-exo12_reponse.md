# Borne d'extrapolation de 100 ms

## Modélisation

Considérons une tête qui tourne à vitesse angulaire constante $\omega = 180^\circ/\text{s} = \pi\text{ rad/s}$. Pour une vitesse constante, l'angle vrai et l'angle extrapolé coïncident :

$$
\theta_{\text{vraie}}(t) = \theta_0 + \omega t
$$

$$
\theta_{\text{extrapolée}}(t) = \theta_0 + \omega t
$$

Dans ce cas idéal, l'erreur angulaire est nulle. En pratique, la vitesse varie. Si l'accélération angulaire $\alpha$ est constante, la pose vraie et l'extrapolation à vitesse initiale $\omega_0$ sont :

$$
\theta_{\text{vraie}}(t) = \theta_0 + \omega_0 t + \frac{1}{2}\alpha t^2
$$

$$
\theta_{\text{extrapolée}}(t) = \theta_0 + \omega_0 t
$$

L'erreur vaut donc :

$$
\epsilon(t) = |\theta_{\text{vraie}}(t) - \theta_{\text{extrapolée}}(t)| = \frac{1}{2}|\alpha|t^2
$$

## Estimation de l'erreur

Avec une accélération angulaire de $15\text{ rad/s}^2$, pour $t = 100\text{ ms} = 0.1\text{ s}$ :

$$
\epsilon(0.1) = \frac{1}{2} \times 15 \times (0.1)^2 = 0.075\text{ rad} \approx 4.3^\circ
$$

Pour $t = 1000\text{ ms} = 1\text{ s}$ :

$$
\epsilon(1) = \frac{1}{2} \times 15 \times 1^2 = 7.5\text{ rad} \approx 430^\circ
$$

Ces estimations illustrent que l'erreur due à une accélération non modélisée croît avec le carré de la durée d'extrapolation. La limite de 100 ms est une borne opérationnelle dans ce scénario, et non une valeur universelle indépendante du mouvement et du matériel.
