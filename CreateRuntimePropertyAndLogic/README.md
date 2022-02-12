<h1>Create Runtime Property and Logic Behind</h1>

This example will show how to make a runtime property and the logic behind it.

This was based on how to pull linked PR's for all ItemRevision objects and generated a property with the values.

**This has been built and tested on TC 13.2 and 13.3**

<h1>Set Release Version</h1>

Set your released version to the proper version. This example was for TC 13.2

Add the view BMIDE, then expand down to the Releases

![image](https://user-images.githubusercontent.com/12979360/153721538-c26c7d97-6aa3-403b-98b0-a7e9757c6387.png)

Expand to your version -> right-click -> Organize -> Set as active Release

![image](https://user-images.githubusercontent.com/12979360/153721623-093e373d-b06e-4b0b-b79b-6e8b852f49db.png)

<h1>Add New Library</h1>

Expand down to the libraries branch in your BMIDE view

![image](https://user-images.githubusercontent.com/12979360/153721707-be37b283-9d4d-4394-99bd-a0937a046336.png)

Right-click -> Add New Library

Add cm and grm as extra dependancies

![image](https://user-images.githubusercontent.com/12979360/153722078-60b61d70-4523-4f1d-90e0-d192f1e3c351.png)

**Perform a BMIDE Template Save**

<h1>Create New Runtime Property</h1>

We will add the new runtime property to our ItemRevision base obejct for this example. Create a basic runtime property for 'TypedReference' as an Array with unlimited and finish it out.

![image](https://user-images.githubusercontent.com/12979360/153721890-8f9534b6-57ff-4a5d-a9b3-fef6369cfcb2.png)

**Perform a save**

Select the new property and go to 'Property Operations' and hit 'Add'

![image](https://user-images.githubusercontent.com/12979360/153722033-5d695540-0631-4eb4-990e-cab176e80808.png)

Check the 'Getter' checkbox and finish out

![image](https://user-images.githubusercontent.com/12979360/153722126-63187aca-6cfd-465b-aaba-702f13b5c8a4.png)

Now your 'Property Operations' should show you the new getter we just added.

![image](https://user-images.githubusercontent.com/12979360/153722171-1566569a-60e6-4b5a-948a-1d2aaff22263.png)

**Perform Save**

<h1>Generate Code Files</h1>
