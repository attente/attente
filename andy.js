function
clear ()
{
  clear ('north-west');
  clear ('north-south');
  clear ('north-east');
  clear ('west-north');
  clear ('west-east');
  clear ('west-south');
  clear ('east-north');
  clear ('east-west');
  clear ('east-south');
  clear ('south-west');
  clear ('south-north');
  clear ('north-west');

  change ();
}

function
change ()
{
  set ('all-north', get ('west-north')
                  + get ('south-north')
                  + get ('east-north'));
  set ('all-west',  get ('north-west')
                  + get ('east-west')
                  + get ('south-west'));
  set ('all-east',  get ('north-east')
                  + get ('west-east')
                  + get ('south-east'));
  set ('all-south', get ('north-south')
                  + get ('west-south')
                  + get ('east-south'));
}

function
get (key)
{
  var val = +(document.getElementById (key).value);
  return val ? val : 0;
}

function
set (key, val)
{
  document.getElementById (key).innerHTML = '' + val;
}

function
clear (key)
{
  document.getElementById (key).value = '';
}
