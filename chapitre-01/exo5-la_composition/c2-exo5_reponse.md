# Composition de poses en 2D

## Représentation d'une pose

Une pose en 2D est représentée par $P = (x, y, \theta)$, où $(x, y)$ est la translation et $\theta$ l'angle de rotation.

## Composition de deux poses

Soient $P_1 = (x_1, y_1, \theta_1)$ et $P_2 = (x_2, y_2, \theta_2)$. Leur composition $P_{12} = P_1 \oplus P_2$ est donnée par :

$$
x_{12} = x_1 + x_2\cos(\theta_1) - y_2\sin(\theta_1)
$$

$$
y_{12} = y_1 + x_2\sin(\theta_1) + y_2\cos(\theta_1)
$$

$$
\theta_{12} = \theta_1 + \theta_2
$$

## Application à un point

Pour un point $p = (x_p, y_p)$ et une pose $P = (x, y, \theta)$, le point transformé $p' = (x', y')$ est :

$$
x' = x + x_p\cos(\theta) - y_p\sin(\theta)
$$

$$
y' = y + x_p\sin(\theta) + y_p\cos(\theta)
$$

## Vérification

On compare l'application successive $p_a = P_2 \otimes (P_1 \otimes p)$ avec l'application de la pose composée $p_b = (P_1 \oplus P_2) \otimes p$. Les deux résultats doivent coïncider, à l'erreur d'arrondi près.

L'écart entre les points est calculé avec la distance euclidienne :

$$
d = \sqrt{(x_a - x_b)^2 + (y_a - y_b)^2}
$$
