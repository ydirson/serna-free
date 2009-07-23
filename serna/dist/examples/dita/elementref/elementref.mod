<!--
 | (C) Copyright IBM Corporation 2001, 2002, 2003. All Rights Reserved.
 | This file is part of the DITA package on IBM's developerWorks site.
 | See license.txt for disclaimers.
 |
 | The Darwin Information Typing Architecture (DITA) was orginated by
 | IBM's XML Workgroup and ID Workbench tools team.
 |
 | Refer to this file by the following public identfier or an appropriate
 | system identifier:
 |
 |   PUBLIC "-//IBM//ELEMENTS DITA Element Reference//EN"
 |
 | Release history (vrm):
 |   1.0.0 Initial use internally, Summer 2001 (pre-dita01 based)
 |   1.0.1 fix 1 for use with DITA10 materials, August 2002
 |   1.1.1 Release 1.1 December 2002
 |   1.1.2 Release 1.2 June 2003
 *-->



<!ENTITY DTDVersion 'V1.1.2' >


<!-- ============ Specialization of declared elements ============ -->
<!ENTITY % elementrefClasses SYSTEM "elementref_class.ent">
<!--%elementrefClasses;-->

<!ENTITY % elementname "elementname">
<!ENTITY % elementdesc "elementdesc">
<!ENTITY % element "element">
<!ENTITY % longname "longname">
<!ENTITY % purpose "purpose">
<!ENTITY % containedby "containedby">
<!ENTITY % contains "contains">
<!ENTITY % attributes "attributes">
<!ENTITY % examples "examples">
<!ENTITY % attlist "attlist">
<!ENTITY % attribute "attribute">
<!ENTITY % attname "attname">
<!ENTITY % attdesc "attdesc">
<!ENTITY % atttype "atttype">
<!ENTITY % attdefvalue "attdefvalue">
<!ENTITY % attrequired "attrequired">

<!ENTITY % elementref-info-types "%info-types;">
<!ENTITY included-domains "">

<!-- extend term to allow use of *element* in its contexts -->
<!ENTITY % term    "term    | %element;">


<!ELEMENT elementref     (%elementname;,  (%titlealts;)?, (%shortdesc;)?, (%prolog;)?,  %elementdesc;, (%related-links;)?, (%elementref-info-types;)* )>
<!ATTLIST elementref      id ID #REQUIRED
                          conref CDATA #IMPLIED
                          %select-atts;
                          outputclass CDATA #IMPLIED
                          xml:lang NMTOKEN #IMPLIED
                          DTDVersion CDATA #FIXED "&DTDVersion;"
                          domains CDATA "&included-domains;"
>

<!ELEMENT elementname    (%element;, (%longname;)?)>
<!ATTLIST elementname     %id-atts;
>

<!ELEMENT element        (#PCDATA | %tm;)*>
<!ATTLIST element         keyref NMTOKEN #IMPLIED
                          %univ-atts;
                          outputclass CDATA #IMPLIED
>

<!ELEMENT longname       (#PCDATA | %tm;)*>
<!ATTLIST longname        keyref NMTOKEN #IMPLIED
                          %univ-atts;
                          outputclass CDATA #IMPLIED
>


<!ELEMENT elementdesc    ((%purpose;)?, (%containedby;)?, (%contains;)?, (%attributes;)?, (%examples;)?)>
<!ATTLIST elementdesc     %univ-atts;
                          outputclass CDATA #IMPLIED
>

<!ELEMENT purpose        (%section.notitle.cnt;)* >
<!ATTLIST purpose
                          %univ-atts;
                          spectitle CDATA "Purpose"
>

<!ELEMENT containedby    (%section.cnt;)* >
<!ATTLIST containedby
                          %univ-atts;
                          spectitle CDATA "Contained by"
>

<!ELEMENT contains       (%section.cnt;)* >
<!ATTLIST contains
                          %univ-atts;
                          spectitle CDATA "Contains"
>

<!ELEMENT attributes     (%attlist; | %state;) >
<!ATTLIST attributes
                          %univ-atts;
                          spectitle CDATA "Attributes"
>

<!ELEMENT examples       (%section.cnt;)* > <!-- programming examples -->
<!ATTLIST examples
                          %univ-atts;
                          spectitle CDATA #IMPLIED
>



<!-- semantic table -->
<!ELEMENT attlist        (%attribute;)+ >
<!ATTLIST attlist         relcolwidth CDATA #IMPLIED
                          keycol NMTOKEN #IMPLIED
                          refcols NMTOKENS #IMPLIED
                          %display-atts;
                          %univ-atts;
                          spectitle CDATA #IMPLIED
                          outputclass CDATA #IMPLIED
>
<!ELEMENT attribute      (%attname;, %attdesc;, %atttype;, %attdefvalue;, %attrequired;) >
<!ATTLIST attribute       %univ-atts;
>
<!ELEMENT attname        (#PCDATA | %basic.ph; )*>       <!-- term -->
<!ATTLIST attname         %univ-atts;
                          specentry CDATA "Name"
>
<!ELEMENT attdesc        (%defn.cnt;)* >        <!-- description -->
<!ATTLIST attdesc         %univ-atts;
                          specentry CDATA "Description"
>
<!ELEMENT atttype        (#PCDATA | %basic.ph; )*>       <!-- type -->
<!ATTLIST atttype         %univ-atts;
                          specentry CDATA "Data Type"
>
<!ELEMENT attdefvalue    (#PCDATA | %basic.ph; )*>     <!-- default value -->
<!ATTLIST attdefvalue     %univ-atts;
                          specentry CDATA "Default Value"
>
<!ELEMENT attrequired    (%boolean; | %state;)*>            <!-- required -->
<!ATTLIST attrequired     %univ-atts;
                          specentry CDATA "Required?"
>

<!--specialization attributes-->

<!ATTLIST elementref      %global-atts; class CDATA "- topic/topic reference/reference elementref/elementref ">
<!ATTLIST elementdesc     %global-atts; class CDATA "- topic/body reference/refbody elementref/elementdesc ">
<!ATTLIST elementname     %global-atts; class CDATA "- topic/title reference/title elementref/elementname ">
<!ATTLIST element         %global-atts; class CDATA "- topic/term elementref/element ">
<!ATTLIST longname        %global-atts; class CDATA "- topic/ph elementref/longname ">
<!ATTLIST purpose         %global-atts; class CDATA "- topic/section elementref/purpose ">
<!ATTLIST containedby     %global-atts; class CDATA "- topic/section reference/refsyn elementref/containedby ">
<!ATTLIST contains        %global-atts; class CDATA "- topic/section reference/refsyn elementref/contains ">
<!ATTLIST attributes      %global-atts; class CDATA "- topic/section elementref/attributes ">
<!ATTLIST examples        %global-atts; class CDATA "- topic/example elementref/examples ">
<!ATTLIST attlist         %global-atts; class CDATA "- topic/simpletable elementref/attlist ">
<!ATTLIST attribute       %global-atts; class CDATA "- topic/strow elementref/attribute ">
<!ATTLIST attname         %global-atts; class CDATA "- topic/stentry elementref/attname ">
<!ATTLIST attdesc         %global-atts; class CDATA "- topic/stentry elementref/attdesc ">
<!ATTLIST atttype         %global-atts; class CDATA "- topic/stentry elementref/atttype ">
<!ATTLIST attdefvalue     %global-atts; class CDATA "- topic/stentry elementref/attdefvalue ">
<!ATTLIST attrequired     %global-atts; class CDATA "- topic/stentry elementref/attrequired ">
