/*
 * COPYRIGHT 2019 SEAGATE LLC
 *
 * THIS DRAWING/DOCUMENT, ITS SPECIFICATIONS, AND THE DATA CONTAINED
 * HEREIN, ARE THE EXCLUSIVE PROPERTY OF SEAGATE TECHNOLOGY
 * LIMITED, ISSUED IN STRICT CONFIDENCE AND SHALL NOT, WITHOUT
 * THE PRIOR WRITTEN PERMISSION OF SEAGATE TECHNOLOGY LIMITED,
 * BE REPRODUCED, COPIED, OR DISCLOSED TO A THIRD PARTY, OR
 * USED FOR ANY PURPOSE WHATSOEVER, OR STORED IN A RETRIEVAL SYSTEM
 * EXCEPT AS ALLOWED BY THE TERMS OF SEAGATE LICENSES AND AGREEMENTS.
 *
 * YOU SHOULD HAVE RECEIVED A COPY OF SEAGATE'S LICENSE ALONG WITH
 * THIS RELEASE. IF NOT PLEASE CONTACT A SEAGATE REPRESENTATIVE
 * http://www.seagate.com/contact
 *
 * Original author:  Abhilekh Mustapure <abhilekh.mustapure@seagate.com>
 * Original creation date: 04-Apr-2019
 */

package com.seagates3.authorization;
import java.io.*;
import java.io.IOException;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;
import org.xml.sax.InputSource;

import com.seagates3.authorization.AccessControlList;

public class AccessControlPolicy {

    private static Document doc;
    Owner owner;
    AccessControlList accessControlList;

    public AccessControlPolicy(File xmlFile) throws ParserConfigurationException,
                      SAXException, IOException {

        DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
        DocumentBuilder dBuilder;
        dBuilder = dbFactory.newDocumentBuilder();
        doc = dBuilder.parse(xmlFile);
        doc.getDocumentElement().normalize();

        loadXml(doc);
    }

    public AccessControlPolicy(String xmlString) throws ParserConfigurationException,
                      SAXException, IOException {
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        DocumentBuilder builder = null;
        builder = factory.newDocumentBuilder();
        doc = builder.parse(new InputSource(new StringReader(xmlString)));
        loadXml(doc);
    }

    // Load ACL elements from XML doc.
    private void loadXml(Document doc) {

        Owner owner = new Owner("", "");
        NodeList ownerNodes = doc.getElementsByTagName("Owner");
        Node firstOwnerNode = ownerNodes.item(0);
        Element firstOwnerElement = (Element)firstOwnerNode;
        Node ownerId = firstOwnerElement.getElementsByTagName("ID").item(0);
        owner.setCanonicalId(ownerId.getTextContent());
        Node ownerDisplayName = firstOwnerElement.getElementsByTagName("DisplayName").item(0);
        owner.setDisplayName(ownerDisplayName.getTextContent());
        this.owner = owner;

        AccessControlList accessControlList = new AccessControlList();
        NodeList accessContolListNodes = doc.getElementsByTagName("AccessControlList");
        Node firstAccessControlListnode = accessContolListNodes.item(0);
        Element firstAccessControlListElement = (Element)firstAccessControlListnode;
        NodeList Grant = firstAccessControlListElement.getElementsByTagName("Grant");

        //Iterates over GrantList
        for (int counter = 0; counter< Grant.getLength(); counter++) {

            Node grantitem = Grant.item(counter);
            Element grantitemelement = (Element)grantitem;
            Grantee grantee = new Grantee("", "");

            Node granteeId = grantitemelement.getElementsByTagName("ID").item(0);
            grantee.setCanonicalId(granteeId.getTextContent());

            Node granteeDisplayname = grantitemelement.getElementsByTagName("DisplayName").item(0);
            grantee.setDisplayName(granteeDisplayname.getTextContent());

            Grant grant = new Grant(grantee, "");
            grant.grantee = grantee;


            Node permission = grantitemelement.getElementsByTagName("Permission").item(0);
            grant.setPermission(permission.getTextContent());
            accessControlList.grantList.add(grant);
        }

        this.accessControlList = accessControlList;

   }

   void setOwner(Owner owner) {
       this.owner.canonicalId = owner.getCanonicalId();
       this.owner.displayName = owner.getDisplayName();
   }

   Owner getOwner() {
       return owner;
   }

   AccessControlList getAccessControlList() {
      return accessControlList;
   }

   void setAccessControlList(AccessControlList acl) {
       this.accessControlList.grantList.clear();
       for(int counter=0; counter<acl.grantList.size();counter++) {
           this.accessControlList.addGrant(acl.grantList.get(counter));
       }
   }

   // Returns ACL XML in string buffer.
   String getXml() throws TransformerException  {
       TransformerFactory tf = TransformerFactory.newInstance();
       Transformer transformer;
       transformer = tf.newTransformer();
       flushXmlValues();
       StringWriter writer = new StringWriter();
       transformer.transform(new DOMSource(doc), new StreamResult(writer));
       String xml = writer.getBuffer().toString();
       return xml;
   }

   private void flushXmlValues(){
       NodeList ownerNodes = doc.getElementsByTagName("Owner");
       Node firstOwnerNode = ownerNodes.item(0);
       Element firstOwnerElement = (Element)firstOwnerNode;

       Node ownerIdNode = firstOwnerElement.getElementsByTagName("ID").item(0);
       ownerIdNode.setTextContent(this.owner.getCanonicalId());

       Node ownerDisplayNameNode = firstOwnerElement.getElementsByTagName("DisplayName").item(0);
       ownerDisplayNameNode.setTextContent(this.owner.getDisplayName());

       Node accessControlListNode = doc.getElementsByTagName("AccessControlList").item(0);
       Element firstAccessControlListElement = (Element)accessControlListNode;

       NodeList GrantNodeList = firstAccessControlListElement.getElementsByTagName("Grant");

       int grantNodesLength = GrantNodeList.getLength();
       int counter = 0;

       for ( counter = 0; counter< this.accessControlList.grantList.size(); counter++) {

           // When grantNodeList length is less than grantlength to be set,adding extra nodes of type grant Node.
           if(counter>(grantNodesLength-1)) {
               Node grantNode = GrantNodeList.item(0);
               Node newNode=grantNode.cloneNode(true);
               accessControlListNode.appendChild(newNode);
           }

           Node grantNode = GrantNodeList.item(counter);
           Element grantElement = (Element)grantNode;

           Node granteeIdNode = grantElement.getElementsByTagName("ID").item(0);
           granteeIdNode.setTextContent(this.accessControlList.grantList.get(counter).grantee.getCanonicalId());

           Node granteeDisplayNameNode = grantElement.getElementsByTagName("DisplayName").item(0);
           granteeDisplayNameNode.setTextContent(this.accessControlList.grantList.get(counter).grantee.getDisplayName());

           Node permissionNode = grantElement.getElementsByTagName("Permission").item(0);
           permissionNode.setTextContent(this.accessControlList.grantList.get(counter).getPermission());

       }

       int indexAtNodeToBeDeleted = counter;

       // When grantNode length is greater than grantlength to be set,removing nodes of type Grant.
       if(grantNodesLength > counter) {
             while((grantNodesLength) != counter) {
                 Node deleteNode = GrantNodeList.item(indexAtNodeToBeDeleted);
                 accessControlListNode.removeChild(deleteNode);
                 counter++;
             }
       }

   }

}