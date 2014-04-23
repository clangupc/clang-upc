[= Autogen5 template upc =]

  __upc_atomic_lock ();
  switch (op_num)
    {[=
  FOR upc_op =][=
    IF (exist? "op_atomic_ok") =][=
      IF (= (get "op_mode") "access") =]
      case [=op_upc_name=]_OP:[=
        CASE op_upc_name =][=
        = 'UPC_GET'    =]
        orig_value = *target_ptr;[=
        = 'UPC_SET'    =]
	if (fetch_ptr != NULL)
	  orig_value = *target_ptr;
	*target_ptr = *operand1;[=
        = 'UPC_CSWAP'  =]
	orig_value = *operand1;
	if (*target_ptr == orig_value)
	  {
	    *target_ptr = *operand2;
	  }
	else
	    orig_value = *target_ptr;[=
        ESAC =]
        break;[=
      ENDIF =][=
      IF (or (and (exist? "type_numeric_op_ok")
                  (= (get "op_mode") "numeric"))
	     (and (exist? "type_bit_op_ok")
                  (= (get "op_mode") "logical"))) =]
      case [=op_upc_name=]_OP:[=
        IF (and (not (exist? "type_floating_point"))
	        (~* (get "op_upc_name")
	            "UPC_(ADD|SUB|INC|DEC|AND|OR|XOR)$")) =][=
          CASE op_upc_name =][=
          ~* 'UPC_(ADD|SUB|AND|OR|XOR)$' =]
	orig_value = *target_ptr;
	*target_ptr [=op_op=]= *operand1;[=
          = 'UPC_INC' =]
	orig_value = *target_ptr;
	*target_ptr += ([=type_c_name=]) 1;[=
          = 'UPC_DEC' =]
	orig_value = *target_ptr;
	*target_ptr -= ([=type_c_name=]) 1;[=
          ESAC =][=
        ELSE =][=
	  (define op_calc "") =][=
          CASE op_upc_name =][=
          ~* 'UPC_(ADD|SUB|AND|OR|XOR)$' =][=
	    (set! op_calc
	      (string-append "orig_value "
	                     (get "op_op")
	  		     " *operand1")) =][=
          ~* 'UPC_(INC|DEC)$' =][=
	    (set! op_calc
	      (string-append "orig_value "
			     (get "op_op")
			     " (" (get "type_c_name") ") 1")) =][=
	  = 'UPC_MULT' =][=
	    (set! op_calc "orig_value * *operand1") =][=
	  = 'UPC_MIN' =][=
	    (set! op_calc
	      "(*operand1 < orig_value) ? *operand1 : orig_value") =][=
	  = 'UPC_MAX' =][=
	    (set! op_calc
	      "(*operand1 > orig_value) ? *operand1 : orig_value") =][=
	  ESAC =]
	orig_value = *target_ptr;
	new_value = [= (. op_calc) =];
        if (orig_value != new_value)
	  *target_ptr = new_value;[=
        ENDIF =]
        break;[=
      ENDIF =][=
    ENDIF =][=
  ENDFOR =]
      default: break;
    }
  __upc_atomic_release ();

