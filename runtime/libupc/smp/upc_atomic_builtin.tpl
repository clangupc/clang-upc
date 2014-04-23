[= Autogen5 template upc =]

  switch (op_num)
    {[=
  FOR upc_op =][=
    IF (exist? "op_atomic_ok") =][=
      IF (= (get "op_mode") "access") =]
      case [=op_upc_name=]_OP:[=
        CASE op_upc_name =][=
        = 'UPC_GET'    =]
        __atomic_load (target_ptr, &orig_value, __ATOMIC_SEQ_CST);[=
        = 'UPC_SET'    =]
	if (fetch_ptr == NULL)
	  __atomic_store (target_ptr, operand1, __ATOMIC_SEQ_CST);
	else
	  __atomic_exchange (target_ptr, operand1, &orig_value,
			     /* memmodel */ __ATOMIC_SEQ_CST);[=
        = 'UPC_CSWAP'  =]
	orig_value = *operand1;
	/* __atomic_compare_exchange will return the previous value
	   in &orig_value independent of whether operand2 is written
	   to the target location.  */[=
	IF (= (get "type_abbrev") "PTS") =]
	op_ok = __atomic_compare_exchange (target_ptr, &orig_value, operand2,
				/* weak */ 0,
				/* success_memmodel */ __ATOMIC_SEQ_CST,
				/* failure_memmodel */ __ATOMIC_SEQ_CST);
	/* If the previous compare exchange operation failed, check
	   for UPC PTS equality (which ignores phase).  If the pointers
	   compare as equal, try again.  */
	if (!op_ok && (orig_value == *operand1))
	  {
            (void) __atomic_compare_exchange (target_ptr,
	                        &orig_value, operand2,
				/* weak */ 0,
				/* success_memmodel */ __ATOMIC_SEQ_CST,
				/* failure_memmodel */ __ATOMIC_SEQ_CST);
	  }[=
	ELSE =]
	(void) __atomic_compare_exchange (target_ptr,
			    &orig_value, operand2,
			    /* weak */ 0,
			    /* success_memmodel */ __ATOMIC_SEQ_CST,
			    /* failure_memmodel */ __ATOMIC_SEQ_CST);[=
	ENDIF =][=
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
	orig_value = __atomic_fetch_[=op_name=] (target_ptr, *operand1,
				__ATOMIC_SEQ_CST);[=
          = 'UPC_INC' =]
	orig_value = __atomic_fetch_add (target_ptr, ([=type_c_name=]) 1,
				__ATOMIC_SEQ_CST);[=
          = 'UPC_DEC' =]
	orig_value = __atomic_fetch_sub (target_ptr, ([=type_c_name=]) 1,
				__ATOMIC_SEQ_CST);[=
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
	do
	  {
            __atomic_load (target_ptr, &orig_value, __ATOMIC_SEQ_CST);
	    new_value = [= (. op_calc) =];
	  }
	while (!__atomic_compare_exchange (target_ptr, &orig_value, &new_value,
				/* weak */ 0,
				/* success_memmodel */ __ATOMIC_SEQ_CST,
				/* failure_memmodel */ __ATOMIC_SEQ_CST));[=
        ENDIF =]
        break;[=
      ENDIF =][=
    ENDIF =][=
  ENDFOR =]
      default: break;
    }

