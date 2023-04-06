# Bug description
The line 158 of the file blur_pvl-3.pvl produces an error.

But looking at the produced code in vpr file (line 990), it is the same as the foralls, but with the things instantiated.

# Python code used to check
```python
_blur_y_s0_y_y = 64 # range
_t34 = (_blur_y_s0_y_y * 8)
_blur_y_s0_y_yi = 4 # range
_t35 = (_blur_y_s0_y_yi + _t34)

_blur_x_s0_y = _t35 + 1 # range

_t37 = ((_blur_x_s0_y - _t34) * 128)
_t36 = (_blur_x_s0_y * 1026)

_blur_x_s0_x_x = 2

min1 = ((_blur_x_s0_y - _blur_y_s0_y_y * 8) * 1024 + 7)
extent1 = 8 * _blur_x_s0_x_x

for xxf in range(min1, min1+extent1):
    if((xxf - ((_blur_x_s0_y - _blur_y_s0_y_y * 8) * 1024 + 7)) % 8 == 0):
        print(xxf)
        print(_blur_x_s0_y * 1026 +
                  ((xxf - ((_blur_x_s0_y - _blur_y_s0_y_y * 8) * 1024 + 7)) //
                  8 *
                  8 +
                  7) +
                  1)
        
min2 = (_blur_x_s0_y - _blur_y_s0_y_y * 8) * 1024
extent2 = 8 * _blur_x_s0_x_x

for xv1f_xxf in range(min2, min2+extent2):
    print(xv1f_xxf)
    print(_blur_x_s0_y * 1026 +
                  ((xv1f_xxf - (_blur_x_s0_y - _blur_y_s0_y_y * 8) * 1024) //
                  8 *
                  8 +
                  (xv1f_xxf - (_blur_x_s0_y - _blur_y_s0_y_y * 8) * 1024) %
                  8) +
                  1)
```

## Same bug for blur_pvl-1.pvl

python code:

```python
take_a_break = False
for _blur_y_s0_y_y in range(0,128):
    if take_a_break:
        break
    for _blur_y_s0_y_yi in range(0,128):
        _t7 = (_blur_y_s0_y_y * 8);
        _t8 = (_blur_y_s0_y_yi + _t7);
        xy = _t8 + 3
        list1 = []
        list2 = []
        listinp1 = []
        listinp1v2 = []
        listinp2 = []
        for xyf in range((_blur_y_s0_y_y*8) + _blur_y_s0_y_yi, xy):
            for xxf in range(0,128):
                for xif in range(0,8):
                    list1.append((((xyf - (_blur_y_s0_y_y*8)) - _blur_y_s0_y_yi)*1024) + ((xxf*8) + xif))
                    listinp1.append((((xyf*1026) + (xxf*8)) + xif) + 1)
                    xy = xyf
                    _t9 = (xy * 1026);
                    xx_x = xxf
                    _t13 = (xx_x * 8);
                    _t11 = (_t13 + _t9);
                    xx_xi = xif
                    _t4 = (xx_xi + _t11);
                    listinp1v2.append(_t4+1)
        for xy in range((_blur_y_s0_y_y*8) + _blur_y_s0_y_yi, ((_blur_y_s0_y_y*8) + _blur_y_s0_y_yi) + 2+1):
            for xx in range(1,1025):
                list2.append(((((xy - (_blur_y_s0_y_y*8)) - _blur_y_s0_y_yi)*1024) + xx) + -1)
                listinp2.append((xy*1026) + xx)
        if(list1 != list2):
            print(1,False)
        if(listinp1 != listinp2):
            print(2,False)
            print(list1[:10])
            print(list2[:10])
            
            print(listinp1[:10])
            print(listinp1v2[:10])
            print(listinp2[:10])
            print(f"_blur_y_s0_y_y: {_blur_y_s0_y_y}, _blur_y_s0_y_yi: {_blur_y_s0_y_yi} ")
            print(len(listinp1), len(listinp2))
            print(listinp1[-10:])
            print(listinp2[-10:])
            print(sum(listinp1), sum(listinp2))
            take_a_break = True
            break
        
```

# Schedule of hist-4 gives something back which does not verify. This is true, I made a mistake somehwere, find out where.