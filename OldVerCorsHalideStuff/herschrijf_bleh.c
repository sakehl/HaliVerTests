//This is what the user puts in:
context 
  ((get_group_id(0)*get_local_size(0)) + get_local_id(0)) <= 99
  ==> Perm(_in[((get_group_id(0)*get_local_size(0)) + get_local_id(0)) + 1], 1\2);

//This gets rewritten to:

context
  (forall* int opencl_gid; 0 <= opencl_gid && opencl_gid < opencl_gcount; 
    (forall* int opencl_lid; 0 <= opencl_lid && opencl_lid < opencl_gsize;
      ((opencl_gid*opencl_gsize) + opencl_lid) <= 99
        ==> Perm(_in[((opencl_gid*opencl_gsize) + opencl_lid) + 1], 1\2)
    )
  );
// -----------------------------------
// We use axiom A2 twice
  axiom A2 {
    e1 <= e2 && e2 < e3 == (e2 \memberof {e1..e3})
  }

context
  (forall* int opencl_gid; (opencl_gid \memberof{0..opencl_gcount}; 
    (forall* int opencl_lid; opencl_lid \memberof{0..opencl_gsize};
      ((opencl_gid*opencl_gsize) + opencl_lid) <= 99
        ==> Perm(_in[((opencl_gid*opencl_gsize) + opencl_lid) + 1], 1\2)
    )
  );
// -----------------------------------
// We use axiom A4:
  axiom A4 {
    (\forall* int i; b1 ; b2 ==> r1 )
    ==
    (\forall* int i; b1 && b2 ; r1 )
  }

context
  (forall* int opencl_gid; (opencl_gid \memberof{0..opencl_gcount} 0; 
    (forall* int opencl_lid; opencl_lid \memberof{0..opencl_gsize} 0
      && ((opencl_gid*opencl_gsize) + opencl_lid) <= 99;
        Perm(_in[((opencl_gid*opencl_gsize) + opencl_lid) + 1], 1\2)
    )
  );
// -----------------------------------
// We use axiom left_right_plus_3_fallback
axiom left_right_plus_3_fallback {
    (\forall* int i; (i \memberof { e1 .. e2 }) && b3; Perm( ar[(e3!i) + i + (e4!i)] , (p1!i) ) )
    ==
    (\forall* int i;  (i \memberof { e1+e3+e4 .. e2+e3+e4 }) && (\let int i=i-e3-e4; b3) ; Perm( ar[i] , p1 ) )
  }

context
  (forall* int opencl_gid; (opencl_gid \memberof{0..opencl_gcount} 0; 
    (forall* int opencl_lid; opencl_lid \memberof{opencl_gid*opencl_gsize + 1 .. opencl_gsize+opencl_gid*opencl_gsize + 1} 0
      && ((opencl_gid*opencl_gsize) + (opencl_lid-opencl_gid*opencl_gsize-1) <= 99;
        Perm(_in[opencl_lid], 1\2)
    )
  );

// -----------------------------------
// We use axiom I1_2
axiom I1_2 {
  e2 + e1 * e2 == (e1+1) * e2
}

context
  (forall* int opencl_gid; (opencl_gid \memberof{0..opencl_gcount}; 
    (forall* int opencl_lid; opencl_lid \memberof{opencl_gid*opencl_gsize + 1 .. (opencl_gid+1)*opencl_gsize + 1} 0
      && ((opencl_gid*opencl_gsize) + (opencl_lid-opencl_gid*opencl_gsize-1) <= 99;
        Perm(_in[opencl_lid], 1\2)
    )
  );


// -----------------------------------
// We use axiom nested_2r_cond_fallback

axiom nested_2r_cond_fallback {
  (\forall* int i;( i \memberof ({ e1 .. e2 }) );
    (\forall* int j;( j \memberof {i * (e3!i) + (e4!i) .. (i + 1) * e3 + (e4!i)} ) && b1;
      (r1!i) ))
  ==
  e3 > 0 ==> (\forall* int j;( j \memberof {e1 * e3 + e4 .. e2 * e3 + e4} ) && (\let int i=(j-e4)/e3; b1); r1 )
}

// i tussen [0,4)
//   j tussen[i*5+1, (i+1)*5+1)

// j [0..5)+1
// j [5..10)+1
// j [10..15)+1
// j [15..20)+1

// j [0, 20)+1 == [1,21)

// 16 -1 = 15
// 15 / 5 = 3 <- group_id was 3

context
  opencl_gsize > 0 ==> (forall* int opencl_lid; (opencl_lid \memberof{1..opencl_gcount*opencl_gsize+1};
    ;&& (\let int opencl_gid=(opencl_lid-1)/opencl_gsize; ((opencl_gid*opencl_gsize) + (opencl_lid-opencl_gid*opencl_gsize-1) <= 99);
        Perm(_in[opencl_lid], 1\2)
    )
  );