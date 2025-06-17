
## Testing Different Pixel Shader Calculations

### Result 1
```hlsl
reflectionVector = reflect(-lightNormal, normal);
reflectionAngle = max(dot(reflectionVector, cameraNormal), 0.0f);
```
![alt text](images/textured_room_1.png)

### Result 2
```hlsl
reflectionVector = reflect(lightVector, normal);
reflectionAngle = max(0, -dot(reflectionNormal, cameraNormal));
```
![alt text](images/textured_room_2.png)

The results are visually indistinguishable.