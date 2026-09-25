# Extrapolation d'une pose 2D

Une pose est définie par $P(t) = (x(t), y(t), \theta(t))$. Le robot avance à une vitesse linéaire constante $v$ dans la direction de son orientation et tourne à une vitesse angulaire constante $\omega$.

## Cas $\omega \neq 0$ : mouvement circulaire

Lorsque la vitesse angulaire n'est pas nulle, la trajectoire suit un arc de cercle de rayon $R = \frac{v}{\omega}$. Le système différentiel est :

$$
\dot{x}(t) = v\cos(\theta(t)), \quad \dot{y}(t) = v\sin(\theta(t)), \quad \dot{\theta}(t) = \omega
$$

Après un intervalle $dt$, la pose extrapolée est :

$$
x(t + dt) = x(t) + \frac{v}{\omega}\left(\sin(\theta(t) + \omega dt) - \sin(\theta(t))\right)
$$

$$
y(t + dt) = y(t) - \frac{v}{\omega}\left(\cos(\theta(t) + \omega dt) - \cos(\theta(t))\right)
$$

$$
theta(t + dt) = theta(t) + omega dt
$$

## Cas $\omega = 0$ : mouvement rectiligne

Lorsque la vitesse angulaire est nulle, le robot avance en ligne droite avec une orientation constante. Cette forme évite la division par zéro dans les équations du mouvement circulaire :

$$
x(t + dt) = x(t) + v\,dt\cos(\theta(t))
$$

$$
y(t + dt) = y(t) + v\,dt\sin(\theta(t))
$$

$$
theta(t + dt) = theta(t)
$$
