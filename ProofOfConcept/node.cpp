#include "arc.h"
#include "node.h"

#include <QPainter>

int Node::NODE_NUMBER=0;

Node::Node(const qreal &x, const qreal &y, const qreal &diameter,const QColor& color,const int importance):
    QGraphicsEllipseItem(x,y,diameter,diameter),
    MyArcs(QVector<Arc*>()),
    info(++NODE_NUMBER),
    myColor(color),
    diameter(diameter)
{

    setPos(x,y);
    //setto i flag in modo tale che l'oggetto si possa muovere e invii segnali alla scena quando cambia posizione
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
    setBrush(myColor);
    this->setZValue(importance);
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * p)
{
    QPen myPen = pen();
    if(isSelected()){

        setBrush(Qt::yellow);
    }
    else
    {
        setBrush(myColor);
    }

    myPen.setColor(myColor);
    painter->setPen(myPen);
    painter->setBrush(myColor);
    QGraphicsEllipseItem::paint(painter,option,p);
    QPointF center=rect().center();
    //stampo il numero informativo al centro
    //fix della posizione della scritta totalmente a caso
    center.setX(center.x()-diameter/16);
    center.setY(center.y()+diameter/16);
    painter->drawText(center,QVariant(info).toString());

}

QVariant Node::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    //se l'oggetto si è si è spostato e è connesso ad una scena(sempre per costruzione
    //però è un controllo che viene fatto per buona prassi)
    if (change == ItemPositionChange&&scene()) {
        //dico ai miei archi di aggiornarsi
        updateArcs();
    }
    return QGraphicsItem::itemChange(change, value);
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    //chiamo a super perchè è lui a mettere le nuove coordinate al nodo grafico quando viene spostato
    QGraphicsEllipseItem::mouseReleaseEvent(event);
    //visto che si possono lanciare gli oggetti circa dopo che ho mollato il mouse potrebbe spostarsi l'oggetto di un pò
    //in realtà non è un vero lancio ma l'evento itemChange avviene ad un determinato frame rate quindi quando mollo il mouse potrebbe
    //non aggiornarsi questo lo obbliga ad un ultimo refresh
    updateArcs();
}


void Node::addLine(Arc *line)
{
    //qt ha un hiding degli oggetti buono quindi non posso passare solo il punto della linea interessato perchè non riesco
    //ad accederci (sono privati quindi manco con l'ereditarietà) per questo motivo passo la linea intera e se questo nodo
    //ha il punto A o il punto B del segmento
    MyArcs.push_back(line);
    //aggiorno il punto con le mie coordinate di centro
}
//rimuove tutte i puntatori uguali
void Node::removeLine(Arc *line)
{
    MyArcs.removeAll(line);
}
//ritorna la lista dei puntatori
const QVector<Arc *> & Node::getArcList() const
{
    return MyArcs;
}
//dice a tutti gli archi che iniziano o finiscono su di lui che si è/stà spostando dicendo di aggiornarsi
void Node::updateArcs()
{
    for(QVector<Arc*>::iterator i=MyArcs.begin();i!=MyArcs.end();++i)
        (*i)->updatePosition();
}
